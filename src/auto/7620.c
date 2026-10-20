#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x30]; s32 x30; u8 pad34[0x160]; s32 x194; } Car;
typedef struct { u8 pad0[0x74]; Car *cars[RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x1058]; u64 x1058; u8 pad1060[0xA0]; Race *x1100; } Game;

typedef struct {
    s16 x0;
    s16 x2;
    s32 x4;
    u8 pad8[4];
    s32 xC;
    u16 x10;
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    f32 x30;
    s32 x34;
    u64 x38;
    s32 x40;
    u8 x44;
    u8 x45;
    Car *x48;
    s32 x4C;
    u8 x50;
} SndReq;

typedef struct { s32 x0; u8 pad[8]; } SndDef;
typedef struct { s32 x0; s32 x4; s32 x8; u8 xC; } SndSlot;

extern Game *gGame;
extern Vec3f D_800B7F20;
extern SndDef D_8007EDC4[];
extern s32 D_801320C8[];
extern s64 D_80131F90[];
extern s32 D_80132164[];
extern u8 D_8007EE48[];
extern volatile SndSlot D_801311D0[];

s16 Obj_NextId(void);
u64 Time_Get(u64 *);
Vec3f Car_GetPos(Car *);
s8 Pool_PushCopy(void *, SndReq *);

void Sound_StartCarLoops(void *pool) {
    SndReq req;
    Vec3f zero = D_800B7F20;
    Car *car;
    s32 i;
    s64 t;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        car = gGame->x1100->cars[i];
        req.x0 = 0x3F4;
        req.x4 = 0;
        req.x2 = Obj_NextId();
        req.x10 = D_8007EDC4[car->x194].x0;
        req.x14 = D_801320C8[req.x10];
        if (D_80131F90[req.x10] != -1) {
            t = D_80131F90[req.x10] + Time_Get(&gGame->x1058);
        } else {
            t = -1;
        }
        req.x38 = t;
        req.x18 = Car_GetPos(car);
        req.x24 = zero;
        req.x30 = D_80132164[req.x10];
        req.x34 = 0;
        req.x40 = 0;
        req.x44 = 1;
        req.x45 = 0;
        req.x4C = -1;
        if (car->x30 == 0) {
            req.x48 = car;
            req.xC = 2;
        } else {
            req.x48 = NULL;
            req.xC = 3;
        }
        req.x50 = D_8007EE48[req.xC];
        if (Pool_PushCopy(pool, &req)) {
            D_801311D0[i].x0 = req.x2;
        } else {
            D_801311D0[i].x0 = 0;
        }
        D_801311D0[i].x4 = 0;
        D_801311D0[i].x8 = 0;
        D_801311D0[i].xC = 0;
    }
}
