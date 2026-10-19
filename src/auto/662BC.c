#include "common.h"
#include "m2c.h"

extern f64 D_800BBFA0;

u8 *func_800656BC(u8 *obj, u8 *src) {
    f32 r = *(f32 *)(obj + 0x6C);
    f32 sq = r * r;
    f32 v = (f32)*src * D_800BBFA0;

    *(f32 *)(obj + 0x20) = sq;
    *(f32 *)(obj + 0x68) = v;
    return src + 1;
}
