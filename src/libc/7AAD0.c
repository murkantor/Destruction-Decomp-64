#include "common.h"
#include "m2c.h"

int strcmp(const char *s1, const char *s2) {
    u32 c1;
    s32 d;
    u32 w1;
    u32 w2;

    if (((s1 - s2) & 3) != 0) {
        for (;;) {
            c1 = *(u8 *)s1++;
            d = c1 - *(u8 *)s2++;
            if ((d != 0) | (c1 == 0)) {
                return d;
            }
        }
    }
    while (((u32)s1 & 3) != 0) {
        c1 = *(u8 *)s1++;
        d = c1 - *(u8 *)s2++;
        if ((d != 0) | (c1 == 0)) {
            return d;
        }
    }
    for (;;) {
        w1 = *(u32 *)s1;
        s1 += 4;
        w2 = *(u32 *)s2;
        s2 += 4;
        d = w1 - w2;
        if (d != 0) {
            if (!(w1 & 0xFF000000)) {
                d = 0xFF000000;
            } else if (!(w1 & 0xFF0000)) {
                d = 0xFFFF0000;
            } else if (w1 & 0xFF00) {
                return d;
            } else {
                d = 0xFFFFFF00;
            }
            return (w1 & d) - (w2 & d);
        }
        if ((w1 << 24) == 0 || (w1 & 0xFF00) == 0 || (w1 & 0xFF0000) == 0 || (w1 & 0xFF000000) == 0) {
            return d;
        }
    }
}
