#include "common.h"
#include "m2c.h"

extern u32 __osPiAccessQueueEnabled;
extern OSMesgQueue __osPiAccessQueue;
extern OSMesg piAccessBuf[];
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);

static inline void __osPiCreateAccessQueue(void) {
    __osPiAccessQueueEnabled = 1;
    osCreateMesgQueue(&__osPiAccessQueue, piAccessBuf, 1);
    osSendMesg(&__osPiAccessQueue, NULL, OS_MESG_NOBLOCK);
}

void __osPiGetAccess(void) {
    OSMesg dummyMesg;

    if (!__osPiAccessQueueEnabled) {
        __osPiCreateAccessQueue();
    }
    osRecvMesg(&__osPiAccessQueue, &dummyMesg, OS_MESG_BLOCK);
}
