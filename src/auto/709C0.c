#include "common.h"
#include "m2c.h"

typedef struct {
    u16 type;
    u8 pri;
    u8 status;
    OSMesgQueue *retQueue;
} OSIoMesgHdr;
typedef struct {
    OSIoMesgHdr hdr;
    u8 pad[0x10];
} OSIoMesg;
typedef struct {
    s32 active;
    OSThread *thread;
    OSMesgQueue *cmdQueue;
    OSMesgQueue *evtQueue;
    OSMesgQueue *acsQueue;
    void *dma;
    void *edma;
} OSDevMgr;
extern OSDevMgr __osViDevMgr;
extern u32 __additional_scanline;
extern OSThread viThread;
extern u8 viThreadStack[];
extern OSMesgQueue viEventQueue;
extern OSMesg viEventBuf[5];
extern OSIoMesg viRetraceMsg;
extern OSIoMesg viCounterMsg;
void viMgrMain(void *);
void __osTimerServicesInit(void);
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osSetEventMesg(s32, OSMesgQueue *, OSMesg);
OSPri osGetThreadPri(OSThread *);
void osSetThreadPri(OSThread *, OSPri);
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void __osViInit(void);
void osStartThread(OSThread *);

void osCreateViManager(OSPri pri) {
    u32 savedMask;
    OSPri oldPri;
    OSPri myPri;

    if (__osViDevMgr.active) {
        return;
    }
    __osTimerServicesInit();
    __additional_scanline = 0;
    osCreateMesgQueue(&viEventQueue, viEventBuf, 5);
    viRetraceMsg.hdr.type = 13;
    viRetraceMsg.hdr.pri = 0;
    viRetraceMsg.hdr.retQueue = NULL;
    viCounterMsg.hdr.type = 14;
    viCounterMsg.hdr.pri = 0;
    viCounterMsg.hdr.retQueue = NULL;
    osSetEventMesg(7, &viEventQueue, &viRetraceMsg);
    osSetEventMesg(3, &viEventQueue, &viCounterMsg);
    oldPri = -1;
    myPri = osGetThreadPri(NULL);
    if (myPri < pri) {
        oldPri = myPri;
        osSetThreadPri(NULL, pri);
    }
    savedMask = __osDisableInt();
    __osViDevMgr.active = 1;
    __osViDevMgr.thread = &viThread;
    __osViDevMgr.cmdQueue = &viEventQueue;
    __osViDevMgr.evtQueue = &viEventQueue;
    __osViDevMgr.acsQueue = NULL;
    __osViDevMgr.dma = NULL;
    __osViDevMgr.edma = NULL;
    osCreateThread(&viThread, 0, viMgrMain, &__osViDevMgr, &viThreadStack[0x1008], pri);
    __osViInit();
    osStartThread(&viThread);
    __osRestoreInt(savedMask);
    if (oldPri != -1) {
        osSetThreadPri(NULL, oldPri);
    }
}
