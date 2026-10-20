#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 m[4][4]; } Mtx;
typedef struct { u8 pad[0x10]; f32 x10; } ScaleInfo;
typedef struct { u8 pad[0xC]; ScaleInfo *xC; } TrackInfo;
typedef struct { u8 pad[0x70]; TrackInfo *x70; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
typedef struct { u8 pad[0x1A8]; s32 x1A8; u8 x1AC; u8 pad1AD[0x1B4 - 0x1AD]; s32 x1B4; u8 pad1B8[4]; } Wheel;
typedef struct {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x30 - 0xE];
    s32 x30;
    u8 pad34[0x364 - 0x34];
    Wheel wheels[4];
    u8 padA54[0xA60 - 0xA54];
    f32 xA60;
    u8 padA64[0xB70 - 0xA64];
    void *xB70[4];
} Car;

extern u8 D_8012455C;
extern s32 D_800958A8;
extern Game *gGame;
extern u8 *D_801245A4;
void Vec3f_Set(Vec3f *, f32, f32, f32);
void Mtx_FromRotPos(Mtx *, void *, Car *, s32, s32);
void Particles_EmitAlongSegment(void *, Vec3f *, Vec3f *, Vec3f, f32, f32, s32, s32, s32);
void Skid_Start(void *);
void Skid_Stop(void *);

void func_8000AD40(Car *car, void *a, void *b)
{
    Vec3f p1;
    Vec3f p2;
    Vec3f zero;
    Mtx m;
    s32 i;

    if (D_8012455C) {
        return;
    }
    if (car->xD) {
        return;
    }
    Vec3f_Set(&zero, 0.0f, 0.0f, 0.0f);
    if (car->xA60 <= 0.0) {
        if (D_800958A8 == 3 && car->x30 != 0) {
            goto wheels;
        }
        if (car->wheels[2].x1AC && car->wheels[2].x1B4 != 2) {
            Mtx_FromRotPos(&m, a, car, 2, 1);
            p1.x = m.m[3][0] * gGame->x1100->x70->xC->x10;
            p1.y = m.m[3][1] * gGame->x1100->x70->xC->x10;
            p1.z = m.m[3][2] * gGame->x1100->x70->xC->x10;
            Mtx_FromRotPos(&m, b, car, 2, 1);
            p2.x = m.m[3][0] * gGame->x1100->x70->xC->x10;
            p2.y = m.m[3][1] * gGame->x1100->x70->xC->x10;
            p2.z = m.m[3][2] * gGame->x1100->x70->xC->x10;
            switch (car->wheels[2].x1A8) {
            case 0:
            case 1:
            case 2:
            case 5:
            case 6:
            case 8:
            case 9:
            case 10:
                Particles_EmitAlongSegment(D_801245A4 + 0xB0, &p1, &p2, zero, 3.0f, 0.5f, 20, 20, 0);
                break;
            case 3:
            case 4:
            case 7:
                Particles_EmitAlongSegment(D_801245A4 + 0xB0, &p1, &p2, zero, 3.0f, 0.5f, 20, 20, 1);
                break;
            }
        }
        if (car->wheels[3].x1AC && car->wheels[3].x1B4 != 2) {
            Mtx_FromRotPos(&m, a, car, 3, 1);
            p1.x = m.m[3][0] * gGame->x1100->x70->xC->x10;
            p1.y = m.m[3][1] * gGame->x1100->x70->xC->x10;
            p1.z = m.m[3][2] * gGame->x1100->x70->xC->x10;
            Mtx_FromRotPos(&m, b, car, 3, 1);
            p2.x = m.m[3][0] * gGame->x1100->x70->xC->x10;
            p2.y = m.m[3][1] * gGame->x1100->x70->xC->x10;
            p2.z = m.m[3][2] * gGame->x1100->x70->xC->x10;
            switch (car->wheels[2].x1A8) {
            case 0:
            case 1:
            case 2:
            case 5:
            case 6:
            case 8:
            case 9:
            case 10:
                Particles_EmitAlongSegment(D_801245A4 + 0xB0, &p1, &p2, zero, 3.0f, 0.5f, 20, 20, 0);
                break;
            case 3:
            case 4:
            case 7:
                Particles_EmitAlongSegment(D_801245A4 + 0xB0, &p1, &p2, zero, 3.0f, 0.5f, 20, 20, 1);
                break;
            }
        }
    wheels:
        if (car->xB70[0]) {
            for (i = 0; i < 4; i++) {
                if (car->wheels[i].x1AC) {
                    Skid_Start(car->xB70[i]);
                } else {
                    Skid_Stop(car->xB70[i]);
                }
            }
        }
    } else if (car->xB70[0]) {
        Skid_Stop(car->xB70[0]);
        Skid_Stop(car->xB70[1]);
        Skid_Stop(car->xB70[2]);
        Skid_Stop(car->xB70[3]);
    }
}
