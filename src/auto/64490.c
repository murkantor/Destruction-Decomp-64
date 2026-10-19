#include "common.h"
#include "m2c.h"

void func_80063890(u8 *p) {
    *(u64 *)(p + 0x0) = 0;
    *(u64 *)(p + 0x8) = 0;
    *(s32 *)(p + 0x10) = 0;
    p[0x20] = 0;
    p[0x21] = 0;
    p[0x22] = 0;
    *(s32 *)(p + 0x24) = 0;
}
