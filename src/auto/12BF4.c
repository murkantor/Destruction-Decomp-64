#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    s8 x0;
    s8 x1;
    s8 x2;
    s8 x3;
    u8 x4;
    s8 x5;
    u8 pad6[2];
    Vec3f pos;
    Vec3f vel;
} Emitter;
typedef struct { u8 pad0[0x10]; f32 x10; } Track;
typedef struct { u8 pad0[0xC]; Track *xC; } TrackInfo;
typedef struct { u8 pad0[0x70]; TrackInfo *x70; } Race;
typedef struct { u8 pad0[0x1074]; u8 x1074; u8 pad1075[0x8B]; Race *race; } Game;
extern Game *gGame;
extern u8 D_8012455C;
extern s32 D_80130248;
extern Emitter D_80083B84[];
extern u8 *D_801245A4;
s32 rand(void);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Fx_SpawnParticlesMode(void *, Vec3f, Vec3f, f32, f32, s32, s32, s32);
void func_80016E60(void *, Vec3f, Vec3f, f32, f32, s32, s32);

void *Track_UpdateAmbientEmitters(void *arg0) {
    Vec3f pos;
    Vec3f vel;
    s8 i;
    f32 n;

    if (gGame->x1074) {
        return arg0;
    }
    if (D_8012455C) {
        return arg0;
    }
    for (i = 0; i < 17; i++) {
        if (D_80083B84[i].x0 != D_80130248) {
            continue;
        }
        if (D_80083B84[i].x5 == 0) {
            Vec3f_Scale(&pos, &D_80083B84[i].pos, gGame->race->x70->xC->x10);
            n = D_80083B84[i].x2 * gGame->race->x70->xC->x10;
            if (D_80083B84[i].x1 == 0) {
                vel.x = D_80083B84[i].vel.x + rand() * (1.0f / 32767.0f) - 0.5;
                vel.y = D_80083B84[i].vel.y;
                vel.z = D_80083B84[i].vel.z + rand() * (1.0f / 32767.0f) - 0.5;
                Vec3f_Scale(&vel, &vel, gGame->race->x70->xC->x10);
                Fx_SpawnParticlesMode(D_801245A4 + 0xB0, pos, vel, n, 15.0f, 10, D_80083B84[i].x4, 0);
            } else {
                vel.x = D_80083B84[i].vel.x + rand() * (1.0f / 32767.0f) * 0.1;
                vel.y = D_80083B84[i].vel.y;
                vel.z = D_80083B84[i].vel.z + rand() * (1.0f / 32767.0f) * 0.1;
                Vec3f_Scale(&vel, &vel, gGame->race->x70->xC->x10);
                func_80016E60(D_801245A4 + 0xB0, pos, vel, n, 15.0f, 7, D_80083B84[i].x4);
            }
        }
        D_80083B84[i].x5 = (D_80083B84[i].x5 + 1) % D_80083B84[i].x3;
    }
    return arg0;
}
