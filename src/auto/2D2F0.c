#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u16 x0;
    u8 pad2[2];
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
} Spawn;
typedef struct { u8 pad0[0x10]; s32 x10; } Ai;
typedef struct {
    u8 pad0[0x18];
    s32 x18;
    u8 pad1C[0x18];
    s32 x34;
    Ai *x38;
    u8 pad3C[0x290 - 0x3C];
    Vec3f x290;
} Car;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    u8 padC[8];
    u8 x14[0x60];
    Car *x74[RACE_CARS];
    s32 xA4;
} Race;
typedef struct {
    u8 pad0[0xC];
    s16 xC;
    u8 padE[0x334 - 0xE];
    u8 x334[0x10A8 - 0x334];
    u8 x10A8[0x10FC - 0x10A8];
    s16 x10FC;
    u8 pad10FE[2];
    Race *x1100;
} Game;
typedef struct { u8 pad0[0x14]; f32 x14; f32 x18; } Plr;
typedef struct { Plr *x0; u8 pad4[0x64]; } Slot68;
typedef struct { u8 pad0[0x38]; s32 x38; } Obj;
typedef struct { u8 pad0[0x3C]; Car *x3C; Car *x40; Car *x44; } Team;
typedef struct { s32 x0; s32 x4; s32 x8; } S12;

extern Game *gGame[];
extern Slot68 D_800958DC[];
extern s8 D_800D31CC[];
extern void *D_8011EEF0;
extern s32 D_80097A14;
extern u8 D_800958A1[];
typedef enum { MODE0, MODE1, MODE2, MODE3 } Mode;
extern Mode D_800958A8;
extern s32 D_80095908;
extern u64 D_80097A00[];
extern s8 D_80125C1C[];
extern s32 D_80095D04[];
extern s32 D_80095D34[];
extern s32 D_80095D64[];
extern s32 D_800958AC;
extern s32 D_80096074;
extern s32 D_800958BC;
extern S12 D_80095D94[];
extern Team *D_80096078;
extern Team *D_8009607C;
extern Team *D_80096080;
extern Team *D_80096084;
typedef struct { s32 v[3]; } V3;
extern V3 D_80097D18[];
extern s32 D_80095894;
extern Vec3f D_800D1B08[];
#ifdef TARGET_PC
/* Slots and cars past the N64's 12 (the doubled field) are kept in
   port/game/field_part1.c; FIELD_SLOTS also covers Race_FindCarSlot's "not found" */
extern s32 gFieldD_80095D04[];
extern s32 gFieldD_80095D34[];
extern s32 gFieldD_80095D64[];
extern S12 gFieldD_80095D94[];
extern s32 D_80095850[];
extern s32 gFieldD_80095850[];
/* TODO: switch to the part-0 change's RACE_CARS replacement of D_800D1B08
   (port/game/field_part0.c), which its readers (20E8C.c, 27590.c) use */
extern Vec3f gFieldD_800D1B08[];
#define FIELD_SLOT(t, i) (*((i) < 12 ? &t[i] : &gField##t[(i) - 12]))
#define FIELD_SLOTS (RACE_CARS + 1)
s32 Port_FieldAnnounceCarsLeft(s32 left);
#else
#define FIELD_SLOT(t, i) t[i]
#define FIELD_SLOTS 12
#endif
extern s32 D_800958B4;
extern s32 D_800958A4;
void Players_Reset(void);
void func_8006300C(Plr *, f32, f32, f32, f32, f32, f32);
void func_8005FA38(void *);
void Clock_Resume(void *);
void func_8002D0E0(s32);
void Players_SyncAll(void *);
Obj *Obj_CreateFromSpawn(Spawn *);
void ObjList_Add(Obj *);
void Tag_PassTo(Car *);
s32 Race_FindCarSlot(s32);
void Standings_Resort(void *, Car *);
void func_80014DC0(void);
void func_8005F884(void *);
void Race_AssignSlotObjects(void);
void Race_PlaceCarsOnGrid(void);
void World_UpdateEffects(void *, f32);
void Car_PhysicsStep(void *, f32);
void func_80062FF0(Plr *);
void Debris_Init(void);
void func_8002BF08(void);
void func_8000BAC0(void);

void Race_Setup(void) {
    Spawn sp1;
    Spawn sp2;
    Race *race;
    s32 k;
    s32 i;
    s32 j;
    s32 saved;
    f32 dt;

    Players_Reset();
    dt = 1.0f / 30.0f;
    switch (gGame[0]->xC) {
    case -1:
        break;
    case 0:
        for (i = 0; i < D_800D31CC[0]; i++) {
            D_800958DC[i].x0->x14 = 1.0f;
            D_800958DC[i].x0->x18 = 4096.0f;
            func_8006300C(D_800958DC[i].x0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
        }
        func_8005FA38(D_8011EEF0);
        Clock_Resume(gGame[0]->x10A8);
        func_8002D0E0(D_80097A14);
        return;
    case 1:
        for (i = 0; i < D_800D31CC[0]; i++) {
            D_800958DC[i].x0->x14 = 1.0f;
            D_800958DC[i].x0->x18 = 4096.0f;
            func_8006300C(D_800958DC[i].x0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
        }
        D_800958A1[0] = 0;
        func_8005FA38(D_8011EEF0);
        D_80095908 = D_800958A8;
        Players_SyncAll(&D_800958A1[0x57]);
        return;
    case 2:
        race = gGame[0]->x1100;
        race->x0 = race->xA4;
        race->x8 = D_800D31CC[0];
        race->x4 = gGame[0]->x1100->xA4 - D_800D31CC[0];
        D_80125C1C[0] = race->x0 - 1;
#ifdef TARGET_PC
        /* Announcer_UpdateRace calls out the cars left, from a table of 11 lines, once
           fewer remain than this: in the doubled field only once they are within the
           N64's field (port/src/field.c). The HUD shows the real count. */
        while (D_80125C1C[0] > 0 && !Port_FieldAnnounceCarsLeft(D_80125C1C[0])) {
            D_80125C1C[0]--;
        }
#endif
        D_80097A00[0] = 0;
        switch (D_800958A8) {
        case 0:
            for (i = 0; i < FIELD_SLOTS; i++) {
                FIELD_SLOT(D_80095D04, i) = -1;
                FIELD_SLOT(D_80095D34, i) = -1;
                FIELD_SLOT(D_80095D64, i) = -1;
            }
            break;
        case 1:
            for (i = 0; i < FIELD_SLOTS; i++) {
                FIELD_SLOT(D_80095D04, i) = -1;
                FIELD_SLOT(D_80095D34, i) = -1;
                FIELD_SLOT(D_80095D64, i) = -1;
            }
            break;
        case 2:
            for (i = 0; i < FIELD_SLOTS; i++) {
                FIELD_SLOT(D_80095D04, i) = -1;
                FIELD_SLOT(D_80095D34, i) = -1;
                FIELD_SLOT(D_80095D64, i) = -1;
            }
            break;
        case 3:
            if (D_800958AC == 1) {
                Obj *o1;
                sp1.x0 = 0xE;
                sp1.x4 = 0xEB2B;
                sp1.x8 = 0;
                sp1.xC = 0;
                sp1.x10 = 0;
                sp1.x14 = 0;
                sp1.x18 = 0;
                sp1.x1C = 0;
                o1 = Obj_CreateFromSpawn(&sp1);
                ObjList_Add(o1);
                D_80096074 = o1->x38;
                Tag_PassTo(NULL);
                if (D_800958BC == 0) {
                    for (i = 0; i < gGame[0]->x1100->xA4; i++) {
                        FIELD_SLOT(D_80095D04, i) = 0;
                        FIELD_SLOT(D_80095D94, i).x0 = 0;
                        FIELD_SLOT(D_80095D94, i).x4 = 0;
                        FIELD_SLOT(D_80095D94, i).x8 = 0;
                    }
                } else {
                    for (i = 0; i < gGame[0]->x1100->xA4; i++) {
#ifdef TARGET_PC
                        s32 slot = Race_FindCarSlot(gGame[0]->x1100->x74[i]->x34);
                        gGame[0]->x1100->x74[i]->x18 = FIELD_SLOT(D_80095D04, slot);
#else
                        gGame[0]->x1100->x74[i]->x18 = D_80095D04[Race_FindCarSlot(gGame[0]->x1100->x74[i]->x34)];
#endif
                        Standings_Resort(race->x14, gGame[0]->x1100->x74[i]);
                    }
                }
            }
            if (D_800958AC == 2) {
                Obj *o2;
                sp2.x0 = 0xF;
                sp2.x4 = 0xEB2C;
                sp2.x8 = 0;
                sp2.xC = 0;
                sp2.x10 = 0;
                sp2.x14 = 0;
                sp2.x18 = 0;
                sp2.x1C = 0;
                o2 = Obj_CreateFromSpawn(&sp2);
                ObjList_Add(o2);
                D_80096078 = (Team *)o2->x38;
                sp2.x0 = 0x11;
                sp2.x4 = 0xEB2D;
                sp2.x8 = 0;
                sp2.xC = 0;
                sp2.x10 = 0;
                sp2.x14 = 0;
                sp2.x18 = 0;
                sp2.x1C = 0;
                o2 = Obj_CreateFromSpawn(&sp2);
                ObjList_Add(o2);
                D_8009607C = (Team *)o2->x38;
                sp2.x0 = 0x10;
                sp2.x4 = 0xEB2E;
                sp2.x8 = 0;
                sp2.xC = 0;
                sp2.x10 = 0;
                sp2.x14 = 0;
                sp2.x18 = 0;
                sp2.x1C = 0;
                o2 = Obj_CreateFromSpawn(&sp2);
                ObjList_Add(o2);
                D_80096080 = (Team *)o2->x38;
                sp2.x0 = 0x12;
                sp2.x4 = 0xEB2F;
                sp2.x8 = 0;
                sp2.xC = 0;
                sp2.x10 = 0;
                sp2.x14 = 0;
                sp2.x18 = 0;
                sp2.x1C = 0;
                o2 = Obj_CreateFromSpawn(&sp2);
                ObjList_Add(o2);
                D_80096084 = (Team *)o2->x38;
                for (i = 0; i < 4; i++) {
                    for (j = 2; j >= 0; j--) {
                        D_80097D18[i].v[j] = -1;
                    }
                }
            }
            if (D_800958AC != 1) {
                for (i = 0; i < FIELD_SLOTS; i++) {
                    FIELD_SLOT(D_80095D04, i) = -1;
                    FIELD_SLOT(D_80095D34, i) = -1;
                    FIELD_SLOT(D_80095D64, i) = -1;
                }
            }
            break;
        }
        func_80014DC0();
        func_8005F884(gGame[4]);
        Race_AssignSlotObjects();
        Race_PlaceCarsOnGrid();
        saved = gGame[0]->x10FC;
        D_80095894 = 1;
        gGame[0]->x10FC = 0x1C;
        for (i = 0; i < 60; i++) {
            World_UpdateEffects(gGame[0]->x334, dt);
            Car_PhysicsStep(gGame[0]->x334, dt);
        }
        D_80095894 = 0;
        for (i = 0; i < gGame[0]->x1100->xA4; i++) {
            FIELD_SLOT(D_800D1B08, i) = gGame[0]->x1100->x74[i]->x290;
        }
        gGame[0]->x10FC = saved;
        for (k = 0; k < D_800D31CC[0]; k++) {
            func_80062FF0(D_800958DC[k].x0);
        }
        func_8005FA38(D_8011EEF0);
        Debris_Init();
        if (D_800958B4 == 8) {
            func_8002BF08();
#ifdef TARGET_PC
            /* it resets the 12 slots the N64 has; the rest likewise */
            for (i = 12; i < FIELD_SLOTS; i++) {
                FIELD_SLOT(D_80095850, i) = -1;
            }
#endif
        }
        if (D_800958A8 == 3 && D_800958AC == 2) {
            if (D_800958A4 == 2) {
                D_80096080->x3C = gGame[0]->x1100->x74[0];
                D_80096080->x40 = gGame[0]->x1100->x74[1];
                gGame[0]->x1100->x74[1]->x38->x10 = 14;
                D_80096080->x44 = gGame[0]->x1100->x74[2];
                gGame[0]->x1100->x74[2]->x38->x10 = 13;
                D_80096084->x3C = gGame[0]->x1100->x74[3];
                D_80096084->x40 = gGame[0]->x1100->x74[4];
                gGame[0]->x1100->x74[4]->x38->x10 = 14;
                D_80096084->x44 = gGame[0]->x1100->x74[5];
                gGame[0]->x1100->x74[5]->x38->x10 = 13;
            } else {
                D_80096080->x3C = gGame[0]->x1100->x74[0];
                D_80096080->x40 = gGame[0]->x1100->x74[1];
                D_80096084->x3C = gGame[0]->x1100->x74[2];
                D_80096084->x40 = gGame[0]->x1100->x74[3];
            }
        }
        func_8000BAC0();
        break;
    }
}
