#include "common.h"
#include "m2c.h"

extern u32 __osBaseCounter;
extern u64 __osCurrentTime;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
u32 osGetCount(void);

u64 osGetTime(void) {
    u32 tmptime;
    u32 elapseCount;
    u64 currentCount;
    register u32 saveMask;

    saveMask = __osDisableInt();
    tmptime = osGetCount();
    elapseCount = tmptime - __osBaseCounter;
    currentCount = __osCurrentTime;
    __osRestoreInt(saveMask);
    return currentCount + elapseCount;
}
