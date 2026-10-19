#include "common.h"
#include "m2c.h"

s32 __osAiDeviceBusy(void) {
    s32 status = *(vu32 *)0xA450000C;

    if (status & 0x80000000) {
        return 1;
    }
    return 0;
}
