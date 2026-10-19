#include "common.h"
#include "m2c.h"

s32 memcmp(u8 *a, u8 *b, u32 n) {
    s32 d;
    u32 m;

    if (n == 0) {
        return 0;
    }
    if (((a - b) & 3) != 0) {
        for (n--; n != -1; n--) {
            d = *a - *b;
        b++;
        a++;
            if (d != 0) {
                return d;
            }
        }
        return 0;
    }
    if ((u32)a & 1) {
        d = *a - *b;
        b++;
        a++;
        if (d != 0) {
            return d;
        }
        n--;
    }
    if (n >= 2 && ((u32)a & 2)) {
        d = *(u16 *)a - *(u16 *)b;
        b += 2;
        a += 2;
        if (d != 0) {
            return d;
        }
        n -= 2;
    }
    m = n;
    for (n = (m >> 2) - 1; n != -1; n--) {
        d = *(u32 *)a - *(u32 *)b;
        b += 4;
        a += 4;
        if (d != 0) {
            return d;
        }
    }
    if (m & 2) {
        d = *(u16 *)a - *(u16 *)b;
        b += 2;
        a += 2;
        if (d != 0) {
            return d;
        }
    }
    if (m & 1) {
        d = *a - *b;
        if (d != 0) {
            return d;
        }
    }
    return 0;
}
