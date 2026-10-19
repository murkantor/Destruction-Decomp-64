#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x0;
    f32 x4;
    f32 x8;
    s32 xC;
} Scale;

void func_800329C0(s32 type, Scale *s) {
    s->xC = type;
    switch (type) {
        case 2:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 0.65f;
            break;
        case 3:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 0.95f;
            break;
        case 4:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 0.8f;
            break;
        case 5:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 1.1f;
            break;
        case 6:
            s->x0 = 0.25f;
            s->x8 = s->x4 = 1.1f;
            break;
        case 8:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 1.1f;
            break;
        case 7:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 0.95f;
            break;
        case 10:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 1.1f;
            break;
        case 0:
        case 1:
        case 9:
        default:
            s->x0 = 0.1f;
            s->x8 = s->x4 = 1.25f;
            break;
    }
}
