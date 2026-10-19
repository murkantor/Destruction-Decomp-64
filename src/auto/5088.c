#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f c; f32 r; } Sphere;
typedef struct { u8 pad[0x5C]; Vec3f pos; } UnkCar;
typedef struct { u8 pad[0x38]; UnkCar *x38; } UnkObj;
f32 Vec3f_DistXZ(Vec3f a, Vec3f b);

s32 Car_ClampToSphere(UnkObj *o, Sphere *s) {
    UnkCar *car = o->x38;
    Vec3f n;
    Vec3f d;
    s32 ret = 0;
    f32 dist = Vec3f_DistXZ(s->c, car->pos);
    if (s->r <= dist) {
        d.x = car->pos.x - s->c.x;
        d.y = car->pos.y - s->c.y;
        d.z = car->pos.z - s->c.z;
        n.x = d.x / dist;
        n.y = d.y / dist;
        n.z = d.z / dist;
        car->pos.x = s->c.x + n.x * (s->r - 5.0f);
        car->pos.y = s->c.y + n.y * (s->r - 5.0f);
        car->pos.z = s->c.z + n.z * (s->r - 5.0f);
        ret = 1;
    }
    return ret;
}
