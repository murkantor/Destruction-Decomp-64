#include "common.h"
#include "m2c.h"

typedef struct {
    s32 active;
    OSThread *thread;
    OSMesgQueue *cmdQueue;
    OSMesgQueue *evtQueue;
    OSMesgQueue *acsQueue;
    s32 (*dma)(s32, u32, void *, u32);
    s32 (*edma)(void *, s32, u32, void *, u32);
} OSDevMgr;
extern OSDevMgr __osPiDevMgr;
extern u32 __osPiAccessQueueEnabled;
extern OSThread piThread;
extern OSMesgQueue __osPiAccessQueue;
extern u8 piThreadStack[];
extern OSMesgQueue piEventQueue;
extern OSMesg piEventBuf;
void func_8006DCF0(void);
void __osPiCreateAccessQueue(void);
void __osDevMgrMain(void *);
s32 osPiRawStartDma(s32, u32, void *, u32);
s32 osEPiRawStartDma(void *, s32, u32, void *, u32);
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osSetEventMesg(s32, OSMesgQueue *, OSMesg);
OSPri osGetThreadPri(OSThread *);
void osSetThreadPri(OSThread *, OSPri);
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void osCreateThread(OSThread *, OSId, void (*)(void *), void *, void *, OSPri);
void osStartThread(OSThread *);

void osCreatePiManager(OSPri pri, OSMesgQueue *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt) {
    u32 savedMask;
    OSPri oldPri;
    OSPri myPri;

    if (__osPiDevMgr.active) {
        return;
    }
    func_8006DCF0();
    osCreateMesgQueue(cmdQ, cmdBuf, cmdMsgCnt);
    osCreateMesgQueue(&piEventQueue, &piEventBuf, 1);
    if (!__osPiAccessQueueEnabled) {
        __osPiCreateAccessQueue();
    }
    osSetEventMesg(8, &piEventQueue, (OSMesg)0x22222222);
    oldPri = -1;
    myPri = osGetThreadPri(NULL);
    if (myPri < pri) {
        oldPri = myPri;
        osSetThreadPri(NULL, pri);
    }
    savedMask = __osDisableInt();
    __osPiDevMgr.active = 1;
    __osPiDevMgr.thread = &piThread;
    __osPiDevMgr.cmdQueue = cmdQ;
    __osPiDevMgr.evtQueue = &piEventQueue;
    __osPiDevMgr.acsQueue = &__osPiAccessQueue;
    __osPiDevMgr.dma = osPiRawStartDma;
    __osPiDevMgr.edma = osEPiRawStartDma;
    osCreateThread(&piThread, 0, __osDevMgrMain, &__osPiDevMgr, &piThreadStack[0x1000], pri);
    osStartThread(&piThread);
    __osRestoreInt(savedMask);
    if (oldPri != -1) {
        osSetThreadPri(NULL, oldPri);
    }
}
