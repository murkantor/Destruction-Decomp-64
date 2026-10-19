#include "common.h"
#include "m2c.h"

u32 osVirtualToPhysical(void *);
void osWritebackDCache(void *, s32);
void osInvalDCache(void *, s32);

s32 __osSiRawStartDma(s32 direction, void *dramAddr) {
    if (*(vu32 *)0xA4800018 & 3) {
        return -1;
    }
    if (direction == 1) {
        osWritebackDCache(dramAddr, 64);
    }
    *(vu32 *)0xA4800000 = osVirtualToPhysical(dramAddr);
    if (direction == 0) {
        *(vu32 *)0xA4800004 = 0x1FC007C0;
    } else {
        *(vu32 *)0xA4800010 = 0x1FC007C0;
    }
    if (direction == 0) {
        osInvalDCache(dramAddr, 64);
    }
    return 0;
}
