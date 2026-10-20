#include "common.h"
#include "m2c.h"

u8 Audio_RandRange(u8);

u8 *func_800654EC(u8 *obj, u8 *src) {
    u8 v = Audio_RandRange(*src++);

    obj[0xB1] = v;
    obj[0xB1] = v + *src;
    return src + 1;
}


u8 *func_8006553C(u8 *obj, u8 *src) {
    u8 v = Audio_RandRange(*src++);

    obj[0xB4] = v;
    obj[0xB4] = v + *src;
    return src + 1;
}


u8 *func_8006558C(u8 *obj, u8 *src) {
    u8 v = Audio_RandRange(*src++);

    obj[0xB5] = v;
    obj[0xB5] = v + *src;
    return src + 1;
}
