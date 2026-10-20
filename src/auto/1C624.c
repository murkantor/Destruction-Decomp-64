#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[2]; u8 x2; } Sel;
typedef struct { s32 x0; s32 x4; } TexEnt;
typedef struct { u8 pad[0x18]; TexEnt *x18; Sel *x1C; Gfx *x20; } Model;
typedef struct { u8 pad[0xB4]; u8 xB4; } Pad3C_C;
typedef struct { u8 pad[0xC]; Pad3C_C *xC; s8 x10; } Ctrl;
typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; s16 x4; u8 x6; } Lights;
typedef struct { u8 pad[0x184]; f32 x184; u8 pad188[0x19C - 0x188]; f32 x19C; u8 pad1A0[0x1A8 - 0x1A0]; u32 x1A8; u8 pad1AC[0x1BC - 0x1AC]; } Wheel;
typedef struct {
    u8 pad[0xC];
    u8 xC;
    u8 xD;
    u8 xE;
    u8 padF;
    s32 x10;
    u8 pad14[0x2C - 0x14];
    Lights *x2C;
    s32 x30;
    u32 x34;
    u8 pad38[0x3C - 0x38];
    Ctrl *x3C;
    u8 pad40[0x50 - 0x40];
    u64 x50;
    u8 pad58[0x90 - 0x58];
    s32 x90;
    u8 pad94[0x190 - 0x94];
    Model *x190;
    u8 pad194[0x224 - 0x194];
    Vec3f x224;
    Vec3f x230;
    u8 pad23C[0x29C - 0x23C];
    Vec3f x29C;
    Vec3f x2A8;
    u8 pad2B4[0x364 - 0x2B4];
    Wheel wheels[4];
    u8 padA54[0xBE1 - 0x364 - 4 * 0x1BC];
    u8 xBE1;
} Car;
typedef struct { s32 x0; s32 x4; s32 x8; u8 padC[4]; s32 x10; u8 x14[1]; } Race;
typedef struct { u8 pad[0x10]; f32 x10; } TrackC;
typedef struct { u8 pad[0xC]; TrackC *xC; } Track70;
typedef struct { u8 pad[0x70]; Track70 *x70; u8 pad74[0xA4 - 0x74]; s32 xA4; } Track;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x10FC - 0x10B0]; s16 x10FC; u8 pad10FE[2]; Track *x1100; } Game;
typedef struct { u8 pad[0x14]; f32 x14; } Hud;
typedef struct { Hud *x0; u8 pad[0x68 - 4]; } HudSlot;
typedef struct { u8 x0; u8 pad[0xB0 - 1]; } RumbleSlot;

extern Game *gGame[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B4;
extern s32 D_800D1C20[];
extern s32 D_80095DF4[];
extern HudSlot D_800958DC[];
extern u8 D_80091B14[];
extern void *gUi;
extern void *D_8011EEF8[];
extern u8 *D_801245A4;
extern s8 D_800D31CC;
extern u8 D_80095628;
extern RumbleSlot D_80125CC0[];
extern void Frame_ToMatrix(f32 m[4][4], Car *, s32);
extern u64 Time_Get(u64 *);
extern void Pad_Rumble(s32 channel, s32 frames, f32 strength);
extern void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, s32 force, s32 unique);
extern void Ui_RemoveElement(void *, s32);
extern Vec3f Car_GetPos(Car *);
extern Vec3f Car_GetVelocity(Car *);
extern void Sound_PlayAt(void *, Vec3f, Vec3f, s32);
extern void Ai_OnCarWrecked(Car *);
extern s32 rand(void);
extern void func_80016F48(void *, Vec3f, Vec3f, f32, f32, s32);
extern void Fx_SpawnParticlesMode(void *, Vec3f, Vec3f, f32, f32, s32, s32, s32);
extern void Car_SetPrimColorFromTrack(Car *);
extern void Car_UpdateTrackSegment(Car *);
extern void func_80020FB0(Car *);
extern f32 Car_GetSpeed(Car *);
extern void Standings_Resort(void *, Car *);

void Car_UpdateWreck(Car *car) {
    Vec3f pos;
    f32 m[4][4];
    Race *race;
    u8 finished;
    u8 s6;
    s32 i;
    f32 rumble;

    race = (Race *)gGame[0]->x1100;
    Frame_ToMatrix(m, car, 1);
    pos.x = m[3][0] * gGame[0]->x1100->x70->xC->x10;
    pos.y = m[3][1] * gGame[0]->x1100->x70->xC->x10;
    pos.z = m[3][2] * gGame[0]->x1100->x70->xC->x10;
    finished = 0;
    s6 = 0;
    if (D_800958A8 != 2 && race->x10 == 0) {
        Time_Get(&gGame[0]->x10A8);
        if (Time_Get(&gGame[0]->x10A8) >= car->x50 && gGame[0]->x10FC == 0x1C) {
            finished = car->xBE1 == 0;
        }
    }
    if (car->xD == 0) {
        if (car->xE != 0) {
            car->xD = 1;
            car->xE = 0;
            car->x90 = 1;
            if (race->x10 == 1) {
                u32 id = car->x34;
                if (D_800958A8 == 3 && D_800958AC == 2) {
                    i = id;
                } else {
                    for (i = 0; i < 12; i++) {
                        if (D_800D1C20[i] == id) {
                            break;
                        }
                    }
                }
                D_80095DF4[i] = gGame[0]->x1100->xA4 - race->x0 + 1;
            }
            race->x0--;
            if (car->x30 == 0) {
                race->x8--;
                Pad_Rumble(car->x3C->x10, 30, 0.9f);
                D_800958DC[car->x3C->x10].x0->x14 = 0.1f;
                if (gGame[0]->x10FC == 0x1C && D_800958A8 == 3 && D_800958AC == 1) {
                    s32 lines[4][3] = { { 0x70, 0x76, 0x7A }, { 0x71, 0x77, 0x7A }, { 0x72, 0x78, 0x7A }, { 0x73, 0x79, 0x7A } };
                    { s32 off = car->x3C->x10 * 3; Announcer_QueueRandom(&lines[0][off], 3, 0, 1, 0); }
                }
                if (D_80091B14[car->x3C->x10]) {
                    D_80091B14[car->x3C->x10] = 0;
                    Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
                }
            } else {
                race->x4--;
            }
            {
                Model *mdl = car->x190;
                s32 tex = mdl->x18[mdl->x1C->x2].x0;
                Gfx *g = mdl->x20;
                Gfx *cur = NULL;
                for (; g != NULL; g++) {
                    u8 op = g->w0 >> 24;
                    if (op == 0xFD) {
                        cur = g;
                    } else if (op == 0xF0) {
                        cur->w1 = tex;
                    } else if (op == 0xDF) {
                        break;
                    }
                }
            }
            for (i = 0; i < 4; i++) {
                if (car->x34 < 12) {
                    car->wheels[i].x184 = 0.26f;
                } else {
                    car->wheels[i].x184 = 0.36f;
                }
            }
            Sound_PlayAt(D_8011EEF8[0], Car_GetPos(car), Car_GetVelocity(car), 10);
            Sound_PlayAt(D_8011EEF8[0], Car_GetPos(car), Car_GetVelocity(car), 2);
            if (D_800958AC != 1 && race->x0 >= 2 && race->x10 == 1) {
                Ai_OnCarWrecked(car);
            }
        } else if (car->x10 < 21) {
            Vec3f p;
            Vec3f v = { 0.0f, 4.0f, 0.0f };
            s32 r1 = (rand() & 7) - 3;
            s32 r2 = rand() % 12 - 6;
            p.x = pos.x + m[0][0] * 15.0f;
            p.y = pos.y + m[0][1] * 15.0f;
            p.z = pos.z + m[0][2] * 15.0f;
            p.x = p.x - m[0][0] * r1 - m[2][0] * r2;
            p.y = p.y - m[0][1] * r1 - m[2][1] * r2;
            p.z = p.z - m[0][2] * r1 - m[2][2] * r2;
            if (car->x10 <= 0) {
                func_80016F48(D_801245A4 + 0xB0, p, v, 8.0f, 0.3f, 9 - D_800D31CC);
            } else {
                Fx_SpawnParticlesMode(D_801245A4 + 0xB0, p, v, 8.0f, 0.3f, 9 - D_800D31CC, 0xBE, 0);
            }
        }
        if (D_800D31CC < 3 && car->x30 == 0) {
            Car_SetPrimColorFromTrack(car);
        }
        if (race->x10 == 0) {
            Car_UpdateTrackSegment(car);
        }
        func_80020FB0(car);
        if (race->x10 == 0 && D_800958A8 != 2 && car->xC) {
          if (finished) {
            f32 spd;
            car->x2C->x0 = 0;
            spd = Car_GetSpeed(car);
            Time_Get(&gGame[0]->x10A8);
            if (spd <= 3.0f) {
                if (car->x30 == 0) {
                    if (car->x3C->xC->xB4 == 7) {
                        return;
                    }
                    car->xE = 0;
                    race->x0--;
                    race->x8--;
                    car->x2C->x0 = 0;
                    car->x2C->x1 = 1;
                    car->x2C->x2 = 0;
                    car->x2C->x3 = 0;
                    car->x2C->x4 = 0;
                    car->x2C->x6 = 0;
                    D_800958DC[car->x3C->x10].x0->x14 = 0.1f;
                    car->x224.x = 0.0f;
                    car->x224.y = 0.0f;
                    car->x224.z = 0.0f;
                    car->x230.x = 0.0f;
                    car->x230.y = 0.0f;
                    car->x230.z = 0.0f;
                    car->x29C.x = 0.0f;
                    car->x29C.y = 0.0f;
                    car->x29C.z = 0.0f;
                    car->x2A8.x = 0.0f;
                    car->x2A8.y = 0.0f;
                    car->x2A8.z = 0.0f;
                    car->x3C->xC->xB4 = 7;
                    if (D_80091B14[car->x3C->x10]) {
                        D_80091B14[car->x3C->x10] = 0;
                        Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
                    }
                } else {
                    car->xE = 1;
                }
            }
          }
        }
    }
    if (D_80095628 && car->x3C != NULL) {
        s6 = 1;
    }
    if (car->xD || (gGame[0]->x10FC == 0x1D && !s6)) {
        car->x2C->x0 = 0;
        car->x2C->x1 = 0;
        car->x2C->x2 = 1;
        car->x2C->x3 = 0;
        car->x2C->x4 = 0;
        car->x2C->x6 = 0;
        if (Car_GetSpeed(car) <= 1.0) {
            s32 n = 0;
            for (i = 0; i < 4; i++) {
                Wheel *w = &car->wheels[i];
                if (w->x19C < 1.0) {
                    n++;
                }
            }
            if (n == 4) {
                car->x224.x = 0.0f;
                car->x224.y = 0.0f;
                car->x224.z = 0.0f;
                car->x230.x = 0.0f;
                car->x230.y = 0.0f;
                car->x230.z = 0.0f;
                car->x29C.x = 0.0f;
                car->x29C.y = 0.0f;
                car->x29C.z = 0.0f;
                car->x2A8.x = 0.0f;
                car->x2A8.y = 0.0f;
                car->x2A8.z = 0.0f;
                if (car->x30 == 0) {
                    car->x3C->xC->xB4 = 7;
                }
            }
        }
    }
    Standings_Resort(race->x14, car);
    if (car->x30 != 0 || race->x10 != 0) {
        return;
    }
    rumble = 0.0f;
    switch (car->wheels[0].x1A8) {
    case 3:
        if (D_800958B4 == 6) {
            break;
        }
    case 2:
    case 7:
    case 8:
        rumble = 0.5f;
        break;
    case 4:
    case 6:
        rumble = 0.6f;
        break;
    case 0:
    case 1:
    case 5:
    case 9:
    case 10:
        rumble = 0.0f;
        break;
    }
    {
        f32 spd = Car_GetSpeed(car);
        if (D_80125CC0[car->x3C->x10].x0 == 0 && rumble != 0.0 && gGame[0]->x10FC == 0x1C && spd > 0.5 && car->xD == 0) {
            Pad_Rumble(car->x3C->x10, 1, rumble);
        }
    }
}
