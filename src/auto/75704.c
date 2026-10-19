#include "common.h"
#include "m2c.h"

extern u32 __osSiAccessQueueEnabled;
extern u8 __osSiAccessQueue[];
extern void *siAccessBuf[];
void osCreateMesgQueue(void *, void **, s32);
s32 osSendMesg(void *, void *, s32);
s32 osRecvMesg(void *, void **, s32);

static inline void __osSiCreateAccessQueue(void) {
    __osSiAccessQueueEnabled = 1;
    osCreateMesgQueue(__osSiAccessQueue, siAccessBuf, 1);
    osSendMesg(__osSiAccessQueue, NULL, 0);
}

void __osSiGetAccess(void) {
    void *dummyMesg;

    if (!__osSiAccessQueueEnabled) {
        __osSiCreateAccessQueue();
    }
    osRecvMesg(__osSiAccessQueue, &dummyMesg, 1);
}
