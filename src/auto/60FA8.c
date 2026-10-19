#include "common.h"
#include "m2c.h"

u8 *func_800603A8(u8 *p, s32 a, s32 b, s32 c, s32 d) {
    *(s32 *)(p + 0x14) = a;
    *(s32 *)(p + 0x18) = b;
    *(s32 *)(p + 0x1C) = c;
    *(s32 *)(p + 0x20) = d;
    return p;
}
