#include "common.h"
#include "m2c.h"

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    char ch;
    u32 cc;
    u32 w;
    u32 x;

    while ((u32)s & 3) {
        if ((ch = *s) == (char)c) {
            last = s;
        }
        if (ch == 0) {
            return (char *)last;
        }
        s++;
    }
    cc = (u8)c;
    cc |= cc << 8;
    cc |= cc << 16;
loop:
    w = *(u32 *)s;
    x = w ^ cc;
    if (!(x & 0xFF000000)) {
        last = s;
    }
    if (!(w & 0xFF000000)) {
        return (char *)last;
    }
    if (!(x & 0xFF0000)) {
        last = s + 1;
    }
    if (!(w & 0xFF0000)) {
        return (char *)last;
    }
    if (!(x & 0xFF00)) {
        last = s + 2;
    }
    if (!(w & 0xFF00)) {
        return (char *)last;
    }
    if (!(x & 0xFF)) {
        last = s + 3;
    }
    if (!(w & 0xFF)) {
        return (char *)last;
    }
    s += 4;
    goto loop;
}
