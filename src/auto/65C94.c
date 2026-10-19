#include "common.h"
#include "m2c.h"

extern f64 D_800BBF90;

u8 *func_80065094(u8 *obj, u8 *src) {
    obj[0xAE] = *src++;
    obj[0xCD] = *src++;
    *(f32 *)(obj + 0x1C) = -(f32)*src / D_800BBF90;
    return src + 1;
}
