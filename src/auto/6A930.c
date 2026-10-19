#include "common.h"
#include "m2c.h"

extern s16 sintable[];

s16 sins(u16 x) {
    s16 val;

    x >>= 4;
    if (x & 0x400) {
        val = sintable[0x3FF - (x & 0x3FF)];
    } else {
        val = sintable[x & 0x3FF];
    }
    if (x & 0x800) {
        return -val;
    } else {
        return val;
    }
}
