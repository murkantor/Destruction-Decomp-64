#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x8C];
    void *x8C;
    void *x90;
} Task;
typedef struct {
    s16 type;
    u8 pad2[2];
    Task *task;
} Msg;

extern OSMesgQueue D_800BCF18;
extern OSMesgQueue D_800BCF58;
extern s32 D_800B0B6C;
extern s32 D_800B0B70;
extern char D_800BBBEC[], D_800BBC0C[], D_800BBC2C[], D_800BBC44[], D_800BBC5C[], D_800BBC7C[];
extern char D_800BBC9C[], D_800BBCC4[], D_800BBCD8[], D_800BBCE0[], D_800BBCE8[], D_800BBCF0[], D_800BBCF8[];
extern s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
extern void osDpGetCounters(u32 *);
extern void osDpSetStatus(u32);
extern u32 osDpGetStatus(void);
extern void func_80056408(void *);
extern void func_80057408(void *, s32, s32, char *);
extern s32 sprintf(char *, const char *, ...);
extern void func_80055F70(void *);
extern void func_80056098(void *);

Task *Gfx_BusyScreen(s32 *out, s32 n) {
    OSMesg msg[2];
    u8 ctx[0xC0];
    char buf[0x80];
    u32 counters[4];
    s32 count = 0;
    u8 failed = 0;
    Task *result = NULL;

    while (count < n || D_800B0B6C > 0) {
        osRecvMesg(&D_800BCF18, msg, 1);
        switch (((Msg *)msg[0])->type) {
            case 1:
                count++;
                D_800B0B70++;
                if (count > 100) {
                    u32 status;
                    func_80056408(ctx);
                    func_80057408(ctx, 30, 60, D_800BBBEC);
                    func_80057408(ctx, 30, 70, D_800BBC0C);
                    sprintf(buf, D_800BBC2C, count);
                    func_80057408(ctx, 30, 80, buf);
                    sprintf(buf, D_800BBC44, D_800B0B6C);
                    func_80057408(ctx, 30, 90, buf);
                    func_80057408(ctx, 30, 100, D_800BBC0C);
                    func_80057408(ctx, 30, 110, D_800BBC5C);
                    func_80057408(ctx, 30, 120, D_800BBC7C);
                    osDpGetCounters(counters);
                    osDpSetStatus(0x3C0);
                    status = osDpGetStatus();
                    sprintf(buf, D_800BBC9C, counters[1] * 100 / counters[0], counters[2] * 100 / counters[0],
                            counters[3] * 100 / counters[0]);
                    func_80057408(ctx, 30, 140, buf);
                    sprintf(buf, D_800BBCC4, (status & 0x10) ? D_800BBCD8 : D_800BBCE0,
                            (status & 0x20) ? D_800BBCE8 : D_800BBCE0, (status & 0x40) ? D_800BBCF0 : D_800BBCE0,
                            (status & 0x100) ? D_800BBCF8 : D_800BBCE0);
                    func_80057408(ctx, 30, 150, buf);
                }
                break;
            case 2:
                result = ((Msg *)msg[0])->task;
                D_800B0B6C--;
                func_80055F70(result->x8C);
                func_80056098(((Msg *)msg[0])->task->x90);
                break;
            case 4:
                failed = 1;
                break;
        }
    }
    if (osRecvMesg(&D_800BCF58, NULL, 0) == 0) {
        failed = 1;
    }
    if (out != NULL) {
        if (failed) {
            *out = -1;
        } else {
            *out = count;
        }
    }
    return result;
}
