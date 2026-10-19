#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x94 - 0x8]; s32 x94; } Obj;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void func_80010B28(Obj *, Vec3f, Vec3f, Vec3f, f32, f32, u16, u8, s32, s32, s32, s32);

void func_80016E60(Obj *o, Vec3f a, Vec3f b, f32 c, f32 d, s32 e, s32 f) {
    Vec3f zero;

    if (o->x4 < o->x94) {
        Vec3f_Set(&zero, 0.0f, 0.0f, 0.0f);
        func_80010B28(o, a, b, zero, c, d, e, f, 0xFF, 0, 2, 2);
    }
}
