#include "common.h"
#include "m2c.h"

extern u64 gGameTime;
u64 Time_Get(u64 *);

void Timer_Start(u8 *obj, s64 delay) {
    *(u64 *)(obj + 0x18) = Time_Get(&gGameTime) + delay;
}
