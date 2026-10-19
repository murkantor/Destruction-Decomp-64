#include "common.h"
#include "m2c.h"

s32 __osSiDeviceBusy(void) {
    register u32 stat = *(vu32 *)0xA4800018;

    if (stat & 0x3) {
        return 1;
    }
    return 0;
}
