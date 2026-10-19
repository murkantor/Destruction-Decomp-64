#include "common.h"
#include "m2c.h"

char *strcpy(char *, const char *);

char *strcat(char *s1, const char *s2) {
    char *s = s1;
    u32 w;

    while ((u32)s1 & 3) {
        if (*s1 == 0) {
            goto copy;
        }
        s1++;
    }
    for (;;) {
        w = *(u32 *)s1;
        if (!(w & 0xFF000000)) {
            break;
        }
        if (!(w & 0xFF0000)) {
            s1 += 1;
            break;
        }
        if (!(w & 0xFF00)) {
            s1 += 2;
            break;
        }
        if (!(w & 0xFF)) {
            s1 += 3;
            break;
        }
        s1 += 4;
    }
copy:
    strcpy(s1, s2);
    return s;
}
