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
extern u8 osViModeTable[][0x50];
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osCreateViManager(OSPri);
void osViSetMode(void *);
void osViBlack(u8);
void osViSetEvent(OSMesgQueue *, OSMesg, u32);
void osSetEventMesg(s32, OSMesgQueue *, OSMesg);
void osCreateThread(OSThread *, OSId, void (*)(void *), void *, void *, OSPri);
void osStartThread(OSThread *);
void __scMain(void *);

void osCreateScheduler(OSSched *sc, void *stack, OSPri priority, u8 mode, u8 numFields) {
    sc->curRSPTask = 0;
    sc->curRDPTask = 0;
    sc->clientList = 0;
    sc->frameCount = 0;
    sc->audioListHead = 0;
    sc->gfxListHead = 0;
    sc->audioListTail = 0;
    sc->gfxListTail = 0;
    sc->retraceMsg.type = 1;
    sc->prenmiMsg.type = 4;
    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, 8);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, 8);
    osCreateViManager(254);
    osViSetMode(&osViModeTable[mode]);
    osViBlack(TRUE);
    osSetEventMesg(4, &sc->interruptQ, (OSMesg)0x29B);
    osSetEventMesg(9, &sc->interruptQ, (OSMesg)0x29C);
    osSetEventMesg(14, &sc->interruptQ, (OSMesg)0x29D);
    osViSetEvent(&sc->interruptQ, (OSMesg)0x29A, numFields);
    osCreateThread(&sc->thread, 4, __scMain, (void *)sc, stack, priority);
    osStartThread(&sc->thread);
}
