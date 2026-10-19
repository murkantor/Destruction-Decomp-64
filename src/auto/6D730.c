#include "common.h"
#include "m2c.h"

extern u32 osRomBase;
u32 osVirtualToPhysical(void *);

s32 osPiRawStartDma(s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    register u32 stat;

    stat = *(vu32 *)0xA4600010;
    while (stat & 3) {
        stat = *(vu32 *)0xA4600010;
    }
    *(vu32 *)0xA4600000 = osVirtualToPhysical(dramAddr);
    *(vu32 *)0xA4600004 = (osRomBase | devAddr) & 0x1FFFFFFF;
    switch (direction) {
        case 0:
            *(vu32 *)0xA460000C = size - 1;
            break;
        case 1:
            *(vu32 *)0xA4600008 = size - 1;
            break;
        default:
            return -1;
    }
    return 0;
}
