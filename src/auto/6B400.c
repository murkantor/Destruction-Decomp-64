#include "common.h"
#include "m2c.h"

extern OSThread *__osRunningThread;
extern OSThread *__osRunQueue;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void __osEnqueueAndYield(OSThread **);
void __osEnqueueThread(OSThread **, OSThread *);
OSThread *__osPopThread(OSThread **);
void __osDispatchThread(void);
void osStartThread(OSThread *);

s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flags) {
    register u32 saveMask = __osDisableInt();

    while (mq->validCount == 0) {
        if (flags == OS_MESG_NOBLOCK) {
            __osRestoreInt(saveMask);
            return -1;
        } else {
            __osRunningThread->state = OS_STATE_WAITING;
            __osEnqueueAndYield(&mq->mtqueue);
        }
    }
    if (msg != NULL) {
        *msg = mq->msg[mq->first];
    }
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;
    if (mq->fullqueue->next != NULL) {
        osStartThread(__osPopThread(&mq->fullqueue));
    }
    __osRestoreInt(saveMask);
    return 0;
}
