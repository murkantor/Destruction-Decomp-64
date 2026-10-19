#include "common.h"
#include "m2c.h"

s32 Seg2D_Intersects(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3, f32 x4, f32 y4) {
    f32 a;
    f32 b;
    f32 c;
    f32 d1;
    f32 d2;

    a = y2 - y1;
    b = x1 - x2;
    c = x2 * y1 - x1 * y2;
    d1 = a * x3 + b * y3 + c;
    d2 = a * x4 + b * y4 + c;
    if (d1 == 0.0) {
        goto second;
    }
    if (d2 == 0.0) {
        goto second;
    }
    if (d1 < 0.0) {
        if (d2 < 0.0) {
            goto ret0;
        }
    }
    if (0.0 < d1) {
        if (0.0 < d2) {
            goto ret0;
        }
    }
second:
    a = y4 - y3;
    b = x3 - x4;
    c = x4 * y3 - x3 * y4;
    d1 = a * x1 + b * y1 + c;
    d2 = a * x2 + b * y2 + c;
    if (d1 == 0.0) {
        return 1;
    }
    if (d2 == 0.0) {
        return 1;
    }
    if (d1 < 0.0) {
        if (d2 < 0.0) {
            goto ret0;
        }
    }
    if (0.0 < d1) {
        if (0.0 < d2) {
            goto ret0;
        }
    }
    return 1;
ret0:
    return 0;
}
