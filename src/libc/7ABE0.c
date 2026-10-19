#include "common.h"
#include "m2c.h"

u32 strlen(const char *);

char *strncat(char *s1, const char *s2, u32 n) {
    char *s;

    if (n == 0) {
        return s1;
    }
    s = s1 + strlen(s1);
    while ((*s++ = *s2++) != 0) {
        if (--n == 0) {
            *s = 0;
            break;
        }
    }
    return s1;
}
