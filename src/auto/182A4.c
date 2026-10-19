#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x90 - 0x8]; s32 x90; Vec3f x94; s32 xA0; } Obj;
typedef struct { u8 pad[0x218]; Vec3f x218; u8 pad2[0x290 - 0x224]; Vec3f x290; } Car;
typedef struct { u8 pad[0x10]; f32 x10; } Cfg;
typedef struct { u8 pad[0xC]; Cfg *xC; } Track;
typedef struct { u8 pad[0x70]; Track *x70; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame;
extern void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void Vec3f_Length(f32 *, Vec3f *);
extern void Vec3f_SetLength(Vec3f *, Vec3f *, f32);
extern void Particle_Spawn(Obj *, Vec3f, Vec3f, Vec3f, f32, f32, s32, s32, s32, s32, s32, s32);

void Particles_EmitTrail(Obj *e, Vec3f off, Car *car) {
    Vec3f a;
    Vec3f b;
    Vec3f p;
    Vec3f v;
    Vec3f zero = { 0.0f, 0.0f, 0.0f };
    Vec3f d;
    Vec3f step;
    f32 len[2];
    f32 t;

    e->x94 = off;
    Vec3f_Add(&a, &car->x218, &off);
    Vec3f_Add(&b, &car->x290, &off);
    Vec3f_Scale(&a, &a, gGame->x1100->x70->xC->x10);
    Vec3f_Scale(&b, &b, gGame->x1100->x70->xC->x10);
    Vec3f_Sub(&d, &b, &a);
    Vec3f_Length(len, &d);
    Vec3f_SetLength(&step, &d, 2.0f);
    v = zero;
    e->x90 = 0;
    p = a;
    for (t = 0.0f; t < len[0]; t += 2.0) {
        if (e->x4 >= e->xA0) {
            break;
        }
        Particle_Spawn(e, p, v, zero, 2.0f, 0.0f, 8, 0x2D, 0xFF, 0, 0, 0);
        Vec3f_Add(&p, &p, &step);
    }
}
