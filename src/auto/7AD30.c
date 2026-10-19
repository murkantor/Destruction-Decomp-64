#include "common.h"
#include "m2c.h"

extern u8 D_800B6FE0[];

s32 toupper(s32 c) {
    if (D_800B6FE0[c & 0xFF] & 4) {
        c -= 0x20;
    }
    return c;
}
