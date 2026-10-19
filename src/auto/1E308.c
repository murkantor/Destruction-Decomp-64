#include "common.h"
#include "m2c.h"

typedef struct { s32 x0, x4, x8, xC, x10; } Zone;
typedef struct {
    u8 pad0[4];
    s32 x4;
    s32 x8;
    f32 xC;
    f32 x10;
    f32 x14;
    f32 x18;
    f32 x1C;
    u8 x20[0x78 - 0x20];
    f32 x78;
    u8 pad7C[0xB0 - 0x7C];
    f32 xB0;
    f32 xB4;
    f32 xB8;
    f32 xBC;
    f32 xC0;
    u8 padC4[0x1E8 - 0xC4];
    Zone x1E8[10];
    u8 pad2B0[0x2B0 - 0x2B0];
} CarStats;
typedef struct { u8 x0, x1, x2, x3; s16 x4; } CarFlags;
typedef struct { u8 pad0[0x20]; void *x20; u8 pad24[0x44]; } PlayerSlot;
typedef struct { u8 x0[0x28]; } Model;
typedef struct {
    s16 x0;
    s16 x2;
    void *x4;
    u8 pad8[4];
    u8 xC;
    u8 xD;
    u8 xE;
    u8 padF;
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
    s32 x20;
    s32 x24;
    s32 x28;
    CarFlags *x2C;
    s32 x30;
    s32 x34;
    u8 pad38[4];
    PlayerSlot *x3C;
    u8 x40;
    u8 pad41[3];
    s32 x44;
    u8 pad48[8];
    u64 x50;
    s32 x58;
    s32 x5C;
    u64 x60;
    u64 x68[3];
    u64 x80;
    s32 x88;
    s32 x8C;
    s32 x90;
    s32 x94;
    u8 x98[0xC0 - 0x98];
    Model xC0[4];
    u8 x160[0x178 - 0x160];
    u8 x178[0x190 - 0x178];
    void *x190;
    u8 x194[0x1EC - 0x194];
    u8 x1EC[0xA64 - 0x1EC];
    s32 xA64;
    Zone xA68[10];
    u8 padB30[0xB30 - 0xB30];
    u8 xB30[12];
    u8 padB3C[0xB6C - 0xB3C];
    s32 xB6C;
    s32 xB70;
    s32 xB74;
    s32 xB78;
    s32 xB7C;
    s32 xB80;
    f32 xB84;
    f32 xB88;
    f32 xB8C;
    f32 xB90;
    f32 xB94;
    f32 xB98;
    f32 xB9C;
    s32 xBA0;
    s32 xBA4;
    f32 xBA8;
    u8 xBAC;
    u8 padBAD[0xBD4 - 0xBAD];
    s32 xBD4;
    s32 xBD8;
    s32 xBDC;
    u8 xBE0;
    u8 xBE1;
    u8 xBE2;
    u8 xBE3;
    u8 xBE4;
    u8 xBE5;
    u8 padBE6[2];
    s32 xBE8;
    CarStats stats;
    u8 padE9C[4];
} Car;
typedef struct { u8 pad0[0xA4]; s32 numCars; } Race;
typedef struct { u8 pad0[0x1100]; Race *race; } Game;
typedef struct { u8 x0, x1, x2, x3, x4; u8 pad5[0x13]; } PadCfg;
typedef struct { u8 pad0[0x20]; s32 x20; } ResObj;

extern Game *gGame;
extern s8 D_800D31CC;
extern s32 D_800958A4[];
extern u32 D_800958B0;
extern u32 D_800958B4[];
extern s32 D_800958C0[];
extern PlayerSlot D_800958D0[];
extern u8 D_800D2E11;
extern u8 D_80124594[];
extern u8 D_80091B14[];
extern PadCfg D_8012540C[];
extern s32 D_80095FF8[];
extern s32 D_80096068;
extern s32 D_8009606C;
extern s32 D_80096070;
extern s32 D_800919F4[];
extern s32 D_80091A54[];
extern u8 D_80124360;
extern Model D_80125C68;
extern s32 D_800D1C20[];

void *Mem_Alloc(u32);
s16 func_80064D70(void);
f64 log(f64);
void Spline_ComputeCoeffs(void *, void *);
void func_80012A20(void *);
void Obj_AllocSubBuffers(void *);
void Shadow_InitMgr(void *);
void RigidBody_Init(void *, void *);
void Car_BuildLightDisplayLists(Car *, CarStats *);
void func_80063F88(void);
s32 Res_Get(s32);
ResObj *Res_GetEnd(s32);
void func_8005F9C0(void *, void *, void *, s32, s32, s32, f32);
void func_8005A970(void *, s32, s32, s32);
void func_8005AA68(void *, void *);
void func_8005AB4C(void *, f32);

typedef struct { s32 v[12]; } Base12;
extern Base12 D_800B8978;
Car *Car_Create(CarStats *s) {
    Base12 base = D_800B8978;
    u8 sp50[8];
    Car *car;
    CarFlags *flags;
    s32 i;
    s32 a = 2;
    s32 b = 0;
    s32 c = 0;
    s32 d = 0;
    s32 e = 0;
    u8 found;
    s32 model;
    s32 n;

    car = Mem_Alloc(0xEA0);
    if (car == NULL) {
        return NULL;
    }
    flags = Mem_Alloc(8);
    car->stats = *s;
    car->x0 = 3;
    car->x2 = func_80064D70();
    car->x4 = car;
    car->xC = 1;
    car->xD = 0;
    car->xE = 0;
    car->x34 = s->x4;
    car->x10 = s->x8;
    car->x14 = 0;
    car->x90 = 0;
    car->x94 = 0;
    car->x1C = 0;
    car->x20 = 0;
    car->x30 = 1;
    car->xB80 = 0;
    car->xB6C = 0;
    car->xB70 = 0;
    car->xB74 = 0;
    car->xB78 = 0;
    car->xB7C = 0;
    car->xBA0 = -1;
    car->xBA4 = 1;
    car->xBA8 = 1000.0f;
    car->x3C = NULL;
    car->xBD4 = 0;
    car->xBD8 = 0;
    car->xBDC = 0;
    car->xBE0 = 0;
    car->xBE1 = 0;
    car->xBE2 = 0;
    car->xBE3 = 0;
    car->xBE4 = 0;
    car->xBE5 = 0;
    car->xBE8 = 0;
    car->xB88 = s->x10;
    car->xB8C = s->x14;
    car->xB84 = s->xC;
    car->xB90 = s->x18;
    car->xB9C = s->x1C;
    car->xB94 = 1.0 / car->xB90;
    car->xB98 = (log(car->xB90) + 1.21) * 0.4314063848144952;
    car->xBAC = 0;
    for (i = 0; i < 12; i++) {
        car->xB30[i] = 0;
    }
    Spline_ComputeCoeffs(car->x194, s->x20);
    for (n = 0; n < D_800D31CC; n++) {
        found = 0;
        if (D_800958A4[1] == 3 && D_800958A4[2] == 2) {
            if (D_800958A4[0] == 2) {
                if ((gGame->race->numCars == 0) & (n == 0)) {
                    found = 1;
                } else if ((gGame->race->numCars == 3) & (n == 1)) {
                    found = 1;
                }
            } else if (gGame->race->numCars == n) {
                found = 1;
            }
        } else if (car->x34 == D_800958C0[n]) {
            found = 1;
        }
        if (found) {
            PlayerSlot *ply = &D_800958D0[n];
            D_800D2E11 = 0;
            for (i = 0; i < 15; i++) {
                D_80124594[i] = 0;
            }
            car->x3C = ply;
            ply->x20 = car;
            car->x30 = 0;
            D_80091B14[n] = 0;
            if (D_800958B4[0] == 2 || D_800958B4[0] == 11) {
                func_80012A20(&car->xB80);
            }
            if (D_800D31CC < 3) {
                Obj_AllocSubBuffers(&car->xB6C);
                Shadow_InitMgr(&car->xB70);
                Shadow_InitMgr(&car->xB74);
                Shadow_InitMgr(&car->xB78);
                Shadow_InitMgr(&car->xB7C);
            }
            if (D_8012540C[n].x0) {
                car->xBE0 = 1;
            }
            if (D_8012540C[n].x1) {
                car->xBE1 = 1;
            }
            if (D_8012540C[n].x2) {
                car->xBE2 = 1;
            }
            if (D_8012540C[n].x4) {
                car->xBE3 = 1;
            }
            break;
        }
    }
    if (car->xBE3) {
        s->xB0 = s->x78 * 8.8;
        s->xB4 = 0.25f;
        s->xC0 = 1.2f;
        s->xB8 = 1.2f;
    }
    RigidBody_Init(car->x1EC, &s->x78);
    if (D_800958A4[1] == 2) {
        car->x8C = 0;
    } else {
        car->x8C = -1;
    }
    car->x60 = 0;
    car->x80 = 0;
    car->x88 = -1;
    if (D_800958B4[0] >= 4) {
        for (i = 0; i < D_80095FF8[D_800958B4[0]]; i++) {
            car->x68[i] = 0;
        }
    } else {
        for (i = 0; i < 3; i++) {
            car->x68[i] = 0;
        }
    }
    switch (D_800958B0) {
        case 0:
            a = 2;
            b = -20;
            c = 30;
            d = 95;
            e = 60;
            break;
        case 1:
            a = 2;
            b = -10;
            c = 35;
            d = 80;
            e = 50;
            break;
        case 2:
            a = 2;
            b = 0;
            c = 40;
            d = 65;
            e = 35;
            break;
        case 3:
            a = 2;
            b = 10;
            c = 50;
            d = 50;
            e = 30;
            break;
    }
    if (car->x30 == 1) {
        if (gGame->race->numCars == D_80096068) {
            a += 5;
            b += 10;
            c = 50;
        }
        if (gGame->race->numCars == D_8009606C) {
            a += 7;
            b += 20;
            c = 55;
            car->xA68[0].x4 = 45;
            car->xA68[0].x8 = 95;
            car->xA68[1].x4 = 50;
            car->xA68[1].x8 = 95;
            car->xA68[2].x4 = 99;
            car->xA68[2].x8 = 65;
            car->xA68[3].x4 = 99;
            car->xA68[3].x8 = 95;
            car->xA68[4].x4 = 50;
            car->xA68[4].x8 = 95;
            car->xA68[5].x4 = 99;
            car->xA68[5].x8 = 65;
            car->xA68[6].x4 = 99;
            car->xA68[6].x8 = 95;
            car->xA68[7].x4 = 95;
            car->xA68[7].x8 = 99;
        }
        if (gGame->race->numCars == D_80096070) {
            a += 9;
            b += 30;
            c = 65;
            car->xA68[0].x4 = 30;
            car->xA68[0].x8 = 80;
            car->xA68[1].x4 = 35;
            car->xA68[1].x8 = 80;
            car->xA68[2].x4 = 85;
            car->xA68[2].x8 = 50;
            car->xA68[3].x4 = 85;
            car->xA68[3].x8 = 80;
            car->xA68[4].x4 = 35;
            car->xA68[4].x8 = 80;
            car->xA68[5].x4 = 85;
            car->xA68[5].x8 = 50;
            car->xA68[6].x4 = 85;
            car->xA68[6].x8 = 80;
            car->xA68[7].x4 = 80;
            car->xA68[7].x8 = 85;
        }
        car->x50 = (base.v[D_800958B4[0]] + a) * 1000000000.0;
        if (D_800958B4[0] == 8) {
            s32 t = a + 6;
            car->x50 = (base.v[D_800958B4[0]] + t) * 1000000000.0;
        }
        if (D_800958B4[0] >= 4) {
            car->x10 += b;
        } else {
            car->x10 += c;
        }
    } else {
        car->x50 = base.v[D_800958B4[0]] * 1000000000.0;
        if (D_800958B4[0] >= 4) {
            car->x10 += d;
        } else {
            car->x10 += e;
        }
    }
    car->x58 = 0;
    car->x5C = 0;
    car->xA64 = 0;
    car->x40 = 0;
    car->x44 = -1;
    car->x28 = 0;
    car->x18 = 0;
    car->x24 = 0;
    flags->x0 = 0;
    flags->x1 = 1;
    flags->x2 = 0;
    flags->x3 = 0;
    flags->x4 = 0;
    car->x2C = flags;
    {
        s32 k;
        for (k = 0; k < 10; k++) {
            car->xA68[k] = s->x1E8[k];
        }
    }
    Car_BuildLightDisplayLists(car, s);
    i = car->x34;
    if (D_800958A4[1] == 3 && D_800958A4[2] == 2) {
        func_80063F88();
    }
    car->x190 = Res_GetEnd(Res_Get(D_800919F4[i]));
    func_8005F9C0(sp50, car->x160, car->x98, ((ResObj *)car->x190)->x20, 0, 0, 0.01f);
    if (D_80124360 == 0) {
        D_80124360 = 1;
        func_8005A970(&D_80125C68, Res_GetEnd(Res_Get(0x4650))->x20, 0, 0);
    }
    for (model = 0; model < 4; model++) {
        func_8005AA68(&car->xC0[model], &D_80125C68);
        func_8005AB4C(&car->xC0[model], 0.01f);
    }
    func_8005A970(car->x178, Res_GetEnd(Res_Get(D_80091A54[i]))->x20, 0, 0);
    if (D_800958A4[1] == 3 && D_800958A4[2] == 2) {
        if (D_800D31CC == 2) {
            switch (gGame->race->numCars) {
                case 0:
                    car->x34 = 0;
                    break;
                case 1:
                    car->x34 = 1;
                    break;
                case 2:
                    car->x34 = 1;
                    break;
                case 3:
                    car->x34 = 2;
                    break;
                case 4:
                    car->x34 = 3;
                    break;
                case 5:
                    car->x34 = 3;
                    break;
            }
        } else {
            car->x34 = gGame->race->numCars;
        }
    }
    D_800D1C20[gGame->race->numCars] = car->x34;
    return car;
}
