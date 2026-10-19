#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3;
typedef struct { u8 pad[0x2B4]; f32 m[3][3]; } Obj;

void func_80032B24(Obj *o, Vec3 *in, Vec3 *out) {
    Vec3 v = *in;
    out->x = v.x * o->m[0][0] + v.y * o->m[0][1] + v.z * o->m[0][2];
    out->y = v.x * o->m[1][0] + v.y * o->m[1][1] + v.z * o->m[1][2];
    out->z = v.x * o->m[2][0] + v.y * o->m[2][1] + v.z * o->m[2][2];
}
