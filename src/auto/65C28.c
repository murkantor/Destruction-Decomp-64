#include "common.h"
#include "m2c.h"

u8 *func_80065028(u8 *obj, u8 *src) {
    u8 hi = *src++;
    u8 lo = *src;

    *(s16 *)(obj + 0xAC) = 0;
    *(s16 *)(obj + 0xAA) = lo | (hi << 8);
    return src + 1;
}
