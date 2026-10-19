#include "common.h"
#include "m2c.h"

typedef u64 OSTime;

typedef struct {
    u32 type;
    u32 flags;
    u64 *ucode_boot;
    u32 ucode_boot_size;
    u64 *ucode;
    u32 ucode_size;
    u64 *ucode_data;
    u32 ucode_data_size;
    u64 *dram_stack;
    u32 dram_stack_size;
    u64 *output_buff;
    u64 *output_buff_size;
    u64 *data_ptr;
    u32 data_size;
    u64 *yield_data_ptr;
    u32 yield_data_size;
} OSTask_t;

typedef struct {
    s16 type;
    u8 pad[2];
    void *task;
} ScMsg;

typedef struct Task {
    struct Task *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    OSTask_t list;
    OSMesgQueue *msgQ;
    void *msg;
    u8 pad58[0x68 - 0x58];
    ScMsg scmsg;
    u8 pad70[0x88 - 0x70];
    u32 x88;
    u8 pad8C[4];
    void *x90;
    u8 pad94[4];
    u64 *x98;
    u64 *x9C;
    u64 *xA0;
    u64 *xA4;
    u8 padA8[4];
    s32 xAC;
    s32 xB0;
    void *xB4;
    char *xB8;
    u8 padBC[4];
} Task;

extern s32 D_800B0B60;
extern s32 D_800B0B64;
extern void *D_800B0B68;
extern s32 D_800B0B6C;
extern s32 D_800B0B70;
extern OSTime D_800B0B78;
extern Task D_800BCFA8[];
extern u64 D_8007ADD0[];
extern u64 D_8007AEA0[];
extern u64 *D_800BCEE8;
extern u64 *D_800BCEEC;
extern u64 *D_800BCEF0;
extern s32 gVideoFifoShift;
extern OSMesgQueue D_800BCF18;
extern OSMesgQueue *D_801238E4;
extern s32 D_800D2E6C;
extern s32 D_80125C58;
extern u8 gParticlePool[];

OSTime osGetTime(void);
void func_800565B8(Task *);
void osWritebackDCache(void *, s32);
void osWritebackDCacheAll(void);
s32 Gfx_CountCommandKind(u64 *, s32, s32);
Task *Gfx_BusyScreen(s32 *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
void osDpGetCounters(u32 *);
void osDpSetStatus(u32);
u32 osDpGetStatus(void);
s32 sprintf(char *, const char *, ...);
void func_80057408(Task *, s32, s32, char *);
void Emitter_FreeAll(void *, void *);

s32 Debug_DrawPerfStats(Task *t, s32 submit, s32 arg2) {
    OSTime now = osGetTime();
    s32 ret;
    Task *task;
    Task *done;

    if (submit == 1) {
        D_800BCFA8[D_800B0B60] = *t;
        task = &D_800BCFA8[D_800B0B60];
        task->xB4 = D_800B0B68;
        D_800B0B68 = NULL;
        D_800B0B60 = (D_800B0B60 + 1) % 3;
        func_800565B8(task);
        task->list.type = 1;
        task->list.flags = 4;
        task->list.ucode_boot = D_8007ADD0;
        task->list.ucode_boot_size = (u8 *)D_8007AEA0 - (u8 *)D_8007ADD0;
        task->list.ucode = task->xA0;
        task->list.ucode_size = 0x1000;
        task->list.ucode_data = task->xA4;
        task->list.ucode_data_size = 0x800;
        task->list.dram_stack = D_800BCEE8;
        task->list.dram_stack_size = 0xA;
        task->list.output_buff = D_800BCEEC;
        task->list.output_buff_size = D_800BCEEC + (0x410 >> gVideoFifoShift);
        task->list.data_ptr = task->x98;
        task->list.data_size = (task->x9C - task->x98) * sizeof(u64);
        task->list.yield_data_ptr = D_800BCEF0;
        task->list.yield_data_size = 0xC00;
        task->next = NULL;
        task->state = 0;
        task->flags = 0x23;
        task->framebuffer = task->x90;
        task->msgQ = &D_800BCF18;
        task->msg = &task->scmsg;
        task->scmsg.type = 2;
        task->scmsg.task = task;
        osWritebackDCache(task, sizeof(Task));
        osWritebackDCacheAll();
        if (task->x88 & 4) {
            task->xAC = Gfx_CountCommandKind(task->x98, 10, 2);
            task->xB0 = Gfx_CountCommandKind(task->x98, 10, 8);
        } else {
            task->xAC = 0;
            task->xB0 = 0;
        }
        done = Gfx_BusyScreen(&ret, arg2);
        osSendMesg(D_801238E4, task, 1);
        D_800B0B6C++;
    } else {
        done = Gfx_BusyScreen(&ret, arg2);
    }
    if (done != NULL) {
        if (done->x88 & 4) {
            char buf[32];
            u32 cnt[4];

            osDpGetCounters(cnt);
            osDpSetStatus(0x3C0);
            osDpGetStatus();
            sprintf(buf, "CMD=%ld%% PIPE=%ld%% TMEM=%ld%%", cnt[1] * 100 / cnt[0], cnt[2] * 100 / cnt[0],
                    cnt[3] * 100 / cnt[0]);
            func_80057408(done, 20, 190, buf);
            if (D_800B0B78 != 0) {
                sprintf(buf, "CPU=%d%%", (s32)(((now - D_800B0B78) * 64) / 3000) * 100 / 16667);
                func_80057408(done, 20, 200, buf);
            }
            if (done->xB8 != NULL) {
                func_80057408(done, 20, 20, done->xB8);
            }
            sprintf(buf, "FPS=%d Frames=%d", 60 / D_800B0B70, D_800B0B70);
            func_80057408(done, 180, 200, buf);
            D_800B0B70 = 0;
            sprintf(buf, "Triangles=%ld", done->xAC);
            func_80057408(done, 20, 210, buf);
            sprintf(buf, "LoadTiles=%ld", done->xB0);
            func_80057408(done, 90, 210, buf);
            sprintf(buf, "Dyn=%ld", D_800D2E6C);
            func_80057408(done, 160, 210, buf);
            sprintf(buf, "Fix=%ld", D_80125C58);
            func_80057408(done, 220, 210, buf);
        }
        if (done->xB4 != NULL) {
            Emitter_FreeAll(done->xB4, gParticlePool);
            done->xB4 = NULL;
        }
        D_800B0B64 = (D_800B0B64 + 1) % 3;
    }
    if (D_800B0B68 != NULL) {
        Emitter_FreeAll(D_800B0B68, gParticlePool);
        D_800B0B68 = NULL;
    }
    D_800B0B78 = osGetTime();
    return ret;
}
