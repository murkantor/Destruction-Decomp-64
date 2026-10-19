#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;
typedef struct {
    u8 pad[0x38];
    Vec3f a;
    u8 pad44[0x10];
    Vec3f b;
} Obj;
void Vec3f_Set(Vec3f *, f32, f32, f32);

void func_8006300C(Obj *o, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz) {
    Vec3f_Set(&o->a, ax, ay, az);
    Vec3f_Set(&o->b, bx, by, bz);
}
