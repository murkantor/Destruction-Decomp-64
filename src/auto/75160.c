#include "common.h"
#include "m2c.h"

void alCopy(void *src, void *dest, s32 len) {
    s32 i;
    u8 *s = src;
    u8 *d = dest;

    for (i = 0; i < len; i++) {
        *d++ = *s++;
    }
}
