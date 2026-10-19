#include "common.h"
#include "m2c.h"

extern char D_800BC1F4[];
extern char D_800BC1F8[];
extern u32 osRomBase;
void __assert(const char *, const char *, int);

s32 osPiRawReadIo(u32 devAddr, u32 *data) {
    register u32 stat;

    if (data == NULL) {
        __assert(D_800BC1F4, D_800BC1F8, 61);
    }
    stat = *(vu32 *)0xA4600010;
    while (stat & 3) {
        stat = *(vu32 *)0xA4600010;
    }
    *data = *(vu32 *)((osRomBase | devAddr) | 0xA0000000);
    return 0;
}
