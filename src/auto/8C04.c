#include "common.h"
#include "m2c.h"

s32 func_80008004(s32 unused, u32 n) {
    s32 t[11] = { 3, 3, 5, 7, 7, 4, 2, 8, 3, 9, 10 };

    if (n > 10) {
        n = 0;
    }
    return t[n];
}
