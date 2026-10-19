#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ramarray[15];
    u32 pifstatus;
} OSPifRam;
typedef struct {
    u8 pad[0x20];
} OSTimer;
extern s32 __osContinitialized;
extern u8 __osMaxControllers;
extern OSPifRam __osContPifRam;
extern u8 __osContLastCmd;
extern OSMesgQueue __osEepromTimerQ;
extern OSMesg __osEepromTimerMsg;
u64 osGetTime(void);
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
s32 osSetTimer(OSTimer *, u64, u64, OSMesgQueue *, OSMesg);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
void __osPackRequestData(u8);
s32 __osSiRawStartDma(s32, void *);
void __osContGetInitData(u8 *, void *);
void __osSiCreateAccessQueue(void);

s32 osContInit(OSMesgQueue *mq, u8 *bitpattern, void *data) {
    OSMesg dummy;
    s32 ret = 0;
    u64 t;
    OSTimer mytimer;
    OSMesgQueue timerMesgQueue;

    if (__osContinitialized) {
        return 0;
    }
    __osContinitialized = 1;
    t = osGetTime();
    if (t < 23437500) {
        osCreateMesgQueue(&timerMesgQueue, &dummy, 1);
        osSetTimer(&mytimer, 23437500 - t, 0, &timerMesgQueue, &dummy);
        osRecvMesg(&timerMesgQueue, &dummy, OS_MESG_BLOCK);
    }
    __osMaxControllers = 4;
    __osPackRequestData(0);
    ret = __osSiRawStartDma(1, &__osContPifRam);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);
    ret = __osSiRawStartDma(0, &__osContPifRam);
    osRecvMesg(mq, &dummy, OS_MESG_BLOCK);
    __osContGetInitData(bitpattern, data);
    __osContLastCmd = 0;
    __osSiCreateAccessQueue();
    osCreateMesgQueue(&__osEepromTimerQ, &__osEepromTimerMsg, 1);
    return ret;
}
