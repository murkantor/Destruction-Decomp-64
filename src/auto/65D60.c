#include "common.h"
#include "m2c.h"

extern f64 D_800BBF98;

u8 *func_80065160(u8 *obj, u8 *src) {
    s32 v = *src++;
    f32 f;

    if (v & 0x80) {
        v |= ~0xFF;
    }
    f = (f32)v / D_800BBF98;
    *(f32 *)(obj + 0x2C) = *(f32 *)(obj + 0x2C) - *(f32 *)(obj + 0x88) + f;
    *(f32 *)(obj + 0x88) = f;
    return src;
}
