#include "common.h"
#include "m2c.h"

s32 __osDpDeviceBusy(void);
u32 osVirtualToPhysical(void *);

s32 osDpSetNextBuffer(void *bufPtr, u64 size) {
    register u32 stat;

    if (__osDpDeviceBusy()) {
        return -1;
    }
    *(vu32 *)0xA410000C = 1;
    do {
        stat = *(vu32 *)0xA410000C;
    } while (stat & 1);
    *(vu32 *)0xA4100000 = osVirtualToPhysical(bufPtr);
    *(vu32 *)0xA4100004 = osVirtualToPhysical(bufPtr) + size;
    return 0;
}
