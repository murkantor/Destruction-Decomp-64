#include "common.h"
#include "m2c.h"

extern OSThread *__osRunningThread;
extern OSThread *__osRunQueue;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void __osDequeueThread(OSThread **, OSThread *);
void __osEnqueueThread(OSThread **, OSThread *);
void __osEnqueueAndYield(OSThread **);

void osSetThreadPri(OSThread *t, OSPri pri) {
    register u32 saveMask = __osDisableInt();

    if (t == NULL) {
        t = __osRunningThread;
    }
    if (t->priority != pri) {
        t->priority = pri;
        if (t != __osRunningThread && t->state != 1) {
            __osDequeueThread(t->queue, t);
            __osEnqueueThread(t->queue, t);
        }
        if (__osRunningThread->priority < __osRunQueue->priority) {
            __osRunningThread->state = 2;
            __osEnqueueAndYield(&__osRunQueue);
        }
    }
    __osRestoreInt(saveMask);
}
