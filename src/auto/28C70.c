#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[0x10]; s8 x10; } PadInfo;
typedef struct {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[2];
    s32 x10;
    u8 pad14[0x1C];
    s32 x30;
    s32 x34;
    u8 pad38[4];
    PadInfo *x3C;
} Car;
typedef struct { u8 pad0[0x16C]; s32 x16C; } Coll;
typedef struct { u8 pad0[0x74]; Car *cars[RACE_CARS]; u8 padA4_[0xA4 - 0x74 - 48]; s32 xA4; } CarList;
typedef struct { u8 pad0[0x10FC]; s16 x10FC; u8 pad10FE[2]; CarList *x1100; } Game;
typedef struct { Car *x0; Car *x4; s32 x8; s32 xC; f32 x10; s32 x14; } HitMsg;

extern Game *gGame;
extern s32 D_80095894;
extern s32 D_800D1C50;
extern s32 D_801260D0;
extern f64 D_800B9010;
extern f64 D_800B9018;
extern f32 D_80095840;
extern u8 *D_801245A4;
extern s32 D_8011EEF8[];

s32 Car_GetHitZone(Car *, Vec3f);
void Car_ApplyDamage(Car *, s32, f32, s32);
void Particles_EmitTrail(void *, Vec3f, Car *);
s32 func_80008004(s32, s32);
void Sound_PlayContact(s32, HitMsg *);
void Car_OnCollision(Car *, s32, Car *, s32, f32);
void Car_DeformMeshAtImpact(Car *, s32, Vec3f, s32);
void Debris_Spawn(Car *, Vec3f, s32);
void Pad_Rumble(s32, s32, f32);

void Car_OnCollision2(Coll *a, Vec3f posA, Coll *b, Vec3f posB, f32 force)
{

    Car *carA = NULL;
    Car *carB = NULL;
    s32 zoneA = 0;
    s32 zoneB = 0;
    s32 i;
    s32 flag;
    s32 hpA;
    s32 hpB;
    s32 level = 0;
    f32 str;
    HitMsg msg;
    Coll *c;


    if (gGame->x10FC != 0x1C && gGame->x10FC != 0x19)
        return;
    if (D_80095894 != 0)
        return;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = (Coll *)((u8 *)gGame->x1100->cars[i] + 0x1EC);
        if (a == c) {
            carA = gGame->x1100->cars[i];
            zoneA = Car_GetHitZone(carA, posA);
        }
        if (b == c) {
            carB = gGame->x1100->cars[i];
            zoneB = Car_GetHitZone(carB, posB);
        }
        if (carA != NULL && carB != NULL)
            break;
    }

    if (D_800D1C50 == carA->x34 && D_801260D0 == carB->x34)
        return;
    if (D_801260D0 == carA->x34 && D_800D1C50 == carB->x34)
        return;
    D_800D1C50 = carA->x34;
    D_801260D0 = carB->x34;

    if (carA == carB) {
        if (force < D_800B9010)
            return;
        if (carB->x10 > 0)
            Car_ApplyDamage(carB, zoneA, force / D_80095840, a->x16C);
        if (zoneA != 0 && zoneA != 7 && zoneA != 8 && zoneA != 9) {
            if (a->x16C == 8 || a->x16C == 5) {
                Particles_EmitTrail(D_801245A4 + 0xC, posA, carB);
            }
            flag = 0;
        } else {
            flag = zoneA != 8;
        }
        msg.x0 = carA;
        msg.x8 = 0;
        msg.x4 = 0;
        msg.xC = func_80008004(D_8011EEF8[0], a->x16C);
        msg.x10 = force;
        msg.x14 = flag;
        Sound_PlayContact(D_8011EEF8[0], &msg);
    } else {
        if (force < D_800B9018)
            return;
        if (carA->xD) {
            Car_ApplyDamage(carB, zoneB, force / D_80095840, 0);
            msg.xC = 4;
            msg.x0 = carB;
            msg.x8 = 0;
            msg.x4 = 0;
            msg.x10 = force;
            msg.x14 = 1;
            Sound_PlayContact(D_8011EEF8[0], &msg);
        }
        if (carB->xD) {
            Car_ApplyDamage(carA, zoneA, force / D_80095840, 0);
            msg.xC = 4;
            msg.x0 = carA;
            msg.x8 = 0;
            msg.x4 = 0;
            msg.x10 = force;
            msg.x14 = 1;
            Sound_PlayContact(D_8011EEF8[0], &msg);
        }
        if (!carA->xD && !carB->xD) {
            hpA = carA->x10;
            hpB = carB->x10;
            Car_OnCollision(carA, zoneA, carB, zoneB, force / D_80095840);
            Car_DeformMeshAtImpact(carA, hpA - carA->x10, posA, zoneA);
            Car_DeformMeshAtImpact(carB, hpB - carB->x10, posB, zoneB);
            if (zoneA != 0 && zoneA != 7 && !(zoneA == 8 || zoneA == 9) && zoneB != 0 &&
                !(zoneB == 7 || zoneB == 8) && zoneB != 9) {
                Debris_Spawn(carA, posA, hpA - carA->x10);
                Debris_Spawn(carB, posB, hpB - carB->x10);
                flag = 0;
            } else {
                flag = 1;
            }
            msg.x0 = carA;
            msg.x8 = zoneA;
            msg.x4 = carB;
            msg.xC = zoneB;
            msg.x10 = force;
            msg.x14 = flag;
            Sound_PlayContact(D_8011EEF8[0], &msg);
        }
    }

    if (force < 10.0f) {
        level = 5;
        str = 0.5f;
    } else if (force < 15.0f) {
        level = 6;
        str = 0.6f;
    } else if (force < 20.0f) {
        level = 7;
        str = 0.7f;
    } else if (force < 25.0f) {
        level = 8;
        str = 0.85f;
    } else {
        str = 0.9f;
        level = 10;
    }
    if (gGame->x10FC == 0x1C) {
        if (carA->x30 == 0 && !carA->xD)
            Pad_Rumble(carA->x3C->x10, level, str);
        if (carB->x30 == 0 && !carB->xD && carA != carB)
            Pad_Rumble(carB->x3C->x10, level, str);
    }
}
