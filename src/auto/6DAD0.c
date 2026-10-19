#include "common.h"
#include "m2c.h"

const u8 *strchr(const u8 *s, int c) {
    const u8 ch = c;

    while (*s != ch) {
        if (*s++ == '\0') {
            return NULL;
        }
    }
    return s;
}
