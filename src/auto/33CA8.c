#include "common.h"
#include "m2c.h"

void func_8003AD40(f32 *, s32, s32, s32);

s32 func_800330A8(f32 *ext, s32 arg1, s32 arg2, f32 radius, s32 id) {
    f32 p[3];
    f32 d;
    f32 t;
    s32 i;

    func_8003AD40(p, arg1, arg2, id);
    d = 0.0f;
    for (i = 0; i < 3; i++) {
        if (p[i] < -ext[i]) {
            t = p[i] + ext[i];
            d += t * t;
        } else if (ext[i] < p[i]) {
            t = p[i] - ext[i];
            d += t * t;
        }
    }
    return d <= radius * radius;
}
