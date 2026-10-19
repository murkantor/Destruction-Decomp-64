#include "common.h"
#include "m2c.h"

extern u8 D_800B56F0;
s32 __osAiDeviceBusy(void);
u32 osVirtualToPhysical(void *);

s32 osAiSetNextBuffer(void *bufPtr, u32 size) {
    char *bptr = bufPtr;

    if (D_800B56F0 != 0) {
        bptr -= 0x2000;
    }
    if ((((u32)bufPtr + size) & 0x1FFF) == 0) {
        D_800B56F0 = 1;
    } else {
        D_800B56F0 = 0;
    }
    if (__osAiDeviceBusy()) {
        return -1;
    }
    *(vu32 *)0xA4500000 = osVirtualToPhysical(bptr);
    *(vu32 *)0xA4500004 = size;
    return 0;
}
