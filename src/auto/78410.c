#include "common.h"
#include "m2c.h"

extern OSThread *__osRunningThread;
extern OSThread *__osRunQueue;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void __osEnqueueAndYield(OSThread **);

void osYieldThread(void) {
    register u32 saveMask = __osDisableInt();

    __osRunningThread->state = 2;
    __osEnqueueAndYield(&__osRunQueue);
    __osRestoreInt(saveMask);
}
