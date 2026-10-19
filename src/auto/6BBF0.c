#include "common.h"
#include "m2c.h"

u32 __osProbeTLB(void *);

u32 osVirtualToPhysical(void *addr) {
    if ((u32)addr >= 0x80000000 && (u32)addr < 0xA0000000) {
        return (u32)addr & 0x1FFFFFFF;
    } else if ((u32)addr >= 0xA0000000 && (u32)addr < 0xC0000000) {
        return (u32)addr & 0x1FFFFFFF;
    } else {
        return __osProbeTLB(addr);
    }
}
