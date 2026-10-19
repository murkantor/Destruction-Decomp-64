#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[4]; s32 x4; u8 pad8[0x8C]; s32 x94; } Emitter16F48;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void func_80010B28(Emitter16F48 *, Vec3f, Vec3f, Vec3f, f32, f32, s32, s32, s32, s32, s32, s32);

void func_80016F48(Emitter16F48 *e, Vec3f pos, Vec3f vel, f32 a, f32 b, s32 c) {
    Vec3f zero;

    if (e->x4 < e->x94) {
        Vec3f_Set(&zero, 0.0f, 0.0f, 0.0f);
        func_80010B28(e, pos, vel, zero, a, b, (u16)c, 50, 255, 0, 1, 1);
    }
}
