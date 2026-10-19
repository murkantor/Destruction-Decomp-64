#include "common.h"
#include "m2c.h"

extern u32 __osTimerCounter;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
u32 osGetCount(void);
void __osSetCompare(u32);

void __osSetTimerIntr(u64 tim) {
    u64 NewTime;
    u32 savedMask = __osDisableInt();

    __osTimerCounter = osGetCount();
    NewTime = __osTimerCounter + tim;
    __osSetCompare(NewTime);
    __osRestoreInt(savedMask);
}
