#include "common.h"
#include "m2c.h"

extern char D_800BC1D0[];
extern char D_800BC1D4[];
void __assert(const char *, const char *, int);
s32 __osSiDeviceBusy(void);

s32 __osSiRawReadIo(u32 devAddr, u32 *data) {
    if (devAddr & 3) {
        __assert(D_800BC1D0, D_800BC1D4, 51);
    }
    if (data == NULL) {
        __assert(D_800BC1D0, D_800BC1D4, 52);
    }
    if (__osSiDeviceBusy()) {
        return -1;
    }
    *data = *(vu32 *)(devAddr | 0xA0000000);
    return 0;
}

extern char D_800BC1E0[];
extern char D_800BC1E4[];

s32 __osSiRawWriteIo(u32 devAddr, u32 data) {
    if (devAddr & 3) {
        __assert(D_800BC1E0, D_800BC1E4, 52);
    }
    if (__osSiDeviceBusy()) {
        return -1;
    }
    *(vu32 *)(devAddr | 0xA0000000) = data;
    return 0;
}
