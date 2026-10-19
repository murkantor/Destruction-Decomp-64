#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x90 - 0x8]; s32 x90; s32 x94; } Obj;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Particle_Spawn(Obj *, Vec3f, Vec3f, Vec3f, f32, f32, u16, u8, s32, s32, s32, s32);

void func_800172D0(Obj *o, Vec3f a, Vec3f b, f32 c, f32 d, s32 e) {
    Vec3f zero;

    if (o->x4 < o->x94) {
        if (++o->x90 > 0) {
            o->x90 = 0;
            Vec3f_Set(&zero, 0.0f, 0.0f, 0.0f);
            Particle_Spawn(o, a, b, zero, c, d, e, 0xFF / e, 0xFF, 0, 0, 5);
        }
    }
}
