#include "common.h"
#include "m2c.h"

u8 *func_800650F0(u8 *obj, u8 *src) {
    u8 c = *src++;

    if (c < 0x80) {
        *(s16 *)(obj + 0xA4) = c;
    } else {
        s16 v = (c & 0x7F) << 8;

        *(s16 *)(obj + 0xA4) = v;
        *(s16 *)(obj + 0xA4) = v + *src++;
    }
    return src;
}
