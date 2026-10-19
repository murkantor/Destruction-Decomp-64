#include "common.h"
#include "m2c.h"

extern u64 D_800C0700;
extern s32 D_800C0708;
extern s32 D_800C070C;
u64 osGetTime(void);

void Timer_Reset(void) {
    osGetTime();
    D_800C0700 = 0;
    D_800C0708 = 0;
    D_800C070C = 0;
}
