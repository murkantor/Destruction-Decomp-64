#include "common.h"
#include "m2c.h"

s32 __osDpDeviceBusy(void) {
    register u32 stat = *(vu32 *)0xA410000C;

    if (stat & 0x100) {
        return 1;
    }
    return 0;
}
