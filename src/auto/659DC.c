#include "common.h"
#include "m2c.h"

u8 *func_80064DDC(u8 *obj, u8 *src) {
    u8 c = *src++;

    obj[0xB0] = c;
    if (c != 0) {
        *(f32 *)(obj + 0x4C) = *(f32 *)(obj + 0x28);
    }
    return src;
}
