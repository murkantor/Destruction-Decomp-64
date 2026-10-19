#include "common.h"
#include "m2c.h"

void *memset(void *s, s32 c, u32 n) {
    u8 *p;
    s32 w;

    if (n != 0) {
        c &= 0xFF;
        c |= c << 8;
        c |= c << 16;
        p = s;
        if ((u32)s & 1) {
            *p++ = c;
            n--;
        }
        if (n >= 2 && ((u32)p & 2)) {
            *(u16 *)p = c;
            p += 2;
            n -= 2;
        }
        for (w = (n >> 2) - 1; w != -1; w--) {
            *(u32 *)p = c;
            p += 4;
        }
        if (n & 2) {
            *(u16 *)p = c;
            p += 2;
        }
        if (n & 1) {
            *p = c;
        }
    }
    return s;
}
