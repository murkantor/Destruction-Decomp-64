#include "common.h"
#include "m2c.h"

s32 __osSpDeviceBusy(void) {
    register u32 stat = *(vu32 *)0xA4040010;

    if (stat & 0x1C) {
        return 1;
    }
    return 0;
}
