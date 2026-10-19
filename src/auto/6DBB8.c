#include "common.h"
#include "m2c.h"

void *memcpy(void *, const void *, u32);

void *proutSprintf(void *dst, const char *src, u32 count) {
    memcpy(dst, src, count);
    return (char *)dst + count;
}
