#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0xC]; Vec3f a; Vec3f b; } Obj;

void Basis_RotateRows(Obj *o, f32 angle) {
    Vec3f a = o->a;
    Vec3f b = o->b;
    f32 sc[2];
    f32 x2;
    f32 sn, cs;

    if (angle > 3.14159265) {
        angle -= (s32)((angle + 3.14159265) * 0.1591549) * 6.283185;
    } else if (angle < -3.14159265) {
        angle += (s32)((3.14159265 - angle) * 0.1591549) * 6.283185;
    }
    x2 = angle * angle;
    cs = 1.0 + x2 * (-0.5 + x2 * (0.04166667 + x2 * (-0.001388889 + x2 * (2.480159e-5 + x2 * (-2.755732e-7 + x2 * 2.087675e-9)))));
    sn = angle * (1.0 + x2 * (-0.1666667 + x2 * (0.008333333 + x2 * (-1.984127e-4 + x2 * (2.755732e-6 - x2 * 2.505211e-8)))));
    o->a.x = cs * a.x + sn * b.x;
    o->a.y = cs * a.y + sn * b.y;
    o->a.z = cs * a.z + sn * b.z;
    o->b.x = -sn * a.x + cs * b.x;
    o->b.y = -sn * a.y + cs * b.y;
    sc[0] = sn;
    sc[1] = cs;
    o->b.z = -sn * a.z + cs * b.z;
}
