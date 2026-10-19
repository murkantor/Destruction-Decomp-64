#include "common.h"
#include "m2c.h"

typedef struct {
    s16 type;
    s8 misc[30];
} OSScMsg;
typedef struct OSScTask_s {
    struct OSScTask_s *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    union {
        struct {
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
        } t;
        long long force_structure_alignment;
    } list;
    OSMesgQueue *msgQ;
    OSMesg msg;
} OSScTask;
typedef struct OSScClient_s {
    struct OSScClient_s *next;
    OSMesgQueue *msgQ;
} OSScClient;
typedef struct {
    OSScMsg retraceMsg;
    OSScMsg prenmiMsg;
    OSMesgQueue interruptQ;
    OSMesg intBuf[8];
    OSMesgQueue cmdQ;
    OSMesg cmdMsgBuf[8];
    OSThread thread;
    OSScClient *clientList;
    OSScTask *audioListHead;
    OSScTask *gfxListHead;
    OSScTask *audioListTail;
    OSScTask *gfxListTail;
    OSScTask *curRSPTask;
    OSScTask *curRDPTask;
    u32 frameCount;
    s32 doAudio;
} OSSched;
extern char D_800BC208[];
extern char D_800BC20C[];
void __assert(const char *, const char *, int);
#define assert(EX, line) ((EX) ? ((void)0) : __assert(D_800BC208, D_800BC20C, line))
void __scHandleRetrace(OSSched *sc);
void __scHandleRSP(OSSched *sc);
void __scHandleRDP(OSSched *sc);
void __scAppendList(OSSched *sc, OSScTask *t);
s32 __scTaskComplete(OSSched *sc, OSScTask *t);
void __scExec(OSSched *sc, OSScTask *sp, OSScTask *dp);
void __scYield(OSSched *sc);
s32 __scSchedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
void osWritebackDCacheAll(void);
void osSpTaskLoad(void *);
void osSpTaskStartGo(void *);
s32 osDpSetNextBuffer(void *, u64);
extern s32 D_800B5714;
extern s32 D_800B5718;

void __scExec(OSSched *sc, OSScTask *sp, OSScTask *dp) {
    int rv;

    assert(sc->curRSPTask == 0, 0x1C4);
    if (sp) {
        if (sp->list.t.type == 2) {
            osWritebackDCacheAll();
        }
        sp->state &= ~(0x10 | 0x20);
        osSpTaskLoad(&sp->list);
        osSpTaskStartGo(&sp->list);
        sc->curRSPTask = sp;
        if (sp == dp) {
            sc->curRDPTask = dp;
        }
    }
    if (dp && (dp != sp)) {
        assert(dp->list.t.output_buff, 0x1D7);
        rv = osDpSetNextBuffer(dp->list.t.output_buff, *dp->list.t.output_buff_size);
        D_800B5714 = 1;
        D_800B5718 = 0;
        assert(rv == 0, 0x1E3);
        sc->curRDPTask = dp;
    }
}
