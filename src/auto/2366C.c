#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x38];
    Vec3f x38;
    u8 pad44[0x10];
    Vec3f x54;
} Cam;
typedef struct {
    u8 pad0[0x218];
    Vec3f x218;
    Vec3f x224;
} Car;
f32 sqrtf(f32);
f32 fabsf(f32);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Camera_UpdateBasis(Cam *);

static inline void approach(Vec3f *cur, Vec3f *goal, f32 speed2, f32 m) {
    Vec3f d;
    f32 dist2;
    f32 t;

    Vec3f_Sub(&d, cur, goal);
    dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
    if (16.0 < dist2) {
        dist2 = 16.0f;
    }
    if (dist2 < 0.001) {
        *cur = *goal;
    } else {
        t = (dist2 < speed2) ? m : sqrtf(fabsf(speed2 / dist2));
        if (t < 0.1) {
            t = 0.1f;
        }
        if (m < t) { t = m; }
        cur->x = (1.0f - t) * cur->x + t * goal->x;
        cur->y = (1.0f - t) * cur->y + t * goal->y;
        cur->z = (1.0f - t) * cur->z + t * goal->z;
    }
}

void func_80022A6C(Cam *cam, Car *car, f32 scale) {
    Vec3f target;
    Vec3f pos;
    Vec3f dir;
    Vec3f vel;
    f32 speed2;
    Vec3f *eye = &cam->x38;
    Vec3f *at = &cam->x54;

    Vec3f_Scale(&vel, &car->x224, scale);
    speed2 = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
    Vec3f_Add(&target, &car->x218, &vel);
    Vec3f_Sub(&dir, eye, &target);
    Vec3f_Normalize(&dir, &dir);
    Vec3f_Scale(&vel, &dir, 10.0f);
    Vec3f_Add(&pos, &target, &vel);
    if (pos.y - target.y < 4.0f) {
        pos.y = target.y + 4.0f;
    }
    approach(eye, &pos, speed2, 1.0f);
    approach(at, &target, speed2, 1.0f);
    Camera_UpdateBasis(cam);
}
