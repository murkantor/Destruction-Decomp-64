#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 v[5]; } F5;
typedef struct { u8 pad0[0x248]; Vec3f x248; } Car;
extern F5 D_800B9020;
void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);
void func_8001FC08(Vec3f *, Car *);
f32 Vec3f_AngleBetween(Vec3f, Vec3f);
f32 Vec3f_CrossYSign(Vec3f, Vec3f);

s32 func_800286AC(Car *car, Vec3f dir) {
    Vec3f fwd;
    Vec3f d;
    F5 t;
    f32 dot;
    f32 angle;
    u8 side;
    s32 i;

    t = D_800B9020;
    Vec3f_Dot(&dot, &dir, &car->x248);
    side = 0;
    if (dot > 0.5f) {
        return 8;
    }
    if (dot < -0.5f) {
        return 9;
    }
    func_8001FC08(&fwd, car);
    d = dir;
    d.y = 0.0f;
    fwd.y = 0.0f;
    angle = Vec3f_AngleBetween(fwd, d);
    if (Vec3f_CrossYSign(fwd, d) > 0.0) {
        side = 1;
    }
    for (i = 0; i < 5; i++) {
        if (angle < t.v[i]) {
            break;
        }
    }
    switch (i) {
    case 0:
        return 0;
    case 1:
        if (side) {
            return 4;
        }
        return 1;
    case 2:
        if (side) {
            return 5;
        }
        return 2;
    case 3:
        if (side) {
            return 6;
        }
        return 3;
    case 4:
        return 7;
    }
    return 0;
}
