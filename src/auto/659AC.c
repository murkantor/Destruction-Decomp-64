#include "common.h"
#include "m2c.h"

u8 *func_80064DAC(u8 *obj, u8 *src) {
    u32 v = *src++;

    if (v & 0x80) {
        v = *src++ | ((v & 0x7F) << 8);
    }
    *(s16 *)(obj + 0xA6) = v;
    return src;
}
