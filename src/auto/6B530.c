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

s32 osSendMesg(OSMesgQueue *mq, OSMesg msg, s32 flags) {
    register u32 saveMask = __osDisableInt();
    register s32 last;

    while (mq->validCount >= mq->msgCount) {
        if (flags == OS_MESG_BLOCK) {
            __osRunningThread->state = OS_STATE_WAITING;
            __osEnqueueAndYield(&mq->fullqueue);
        } else {
            __osRestoreInt(saveMask);
            return -1;
        }
    }
    last = (mq->first + mq->validCount) % mq->msgCount;
    mq->msg[last] = msg;
    mq->validCount++;
    if (mq->mtqueue->next != NULL) {
        osStartThread(__osPopThread(&mq->mtqueue));
    }
    __osRestoreInt(saveMask);
    return 0;
}
