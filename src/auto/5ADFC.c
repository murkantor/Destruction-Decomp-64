#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} V3;
f32 cosf(f32);
f32 sinf(f32);

void func_8005A1FC(V3 *out, f32 angle) {
    out->x = cosf(angle);
    out->y = sinf(angle);
    out->z = 0.0f;
}
