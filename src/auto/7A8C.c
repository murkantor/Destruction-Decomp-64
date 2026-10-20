#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x1058]; u64 x1058; } Game;

typedef struct {
    s16 x0;
    s16 x2;
    s32 x4;
    s32 x8;
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
    s32 x48;
    s32 x4C;
    u8 x50;
} SndReq;

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} Info;

typedef struct {
    u8 pad0[0x30];
    s32 x30;
    u8 pad34[0x3C - 0x34];
    Info *x3C;
} Car;

typedef struct {
    Car *x0;
    Car *x4;
    s32 x8;
    s32 xC;
    f32 x10;
    s32 x14;
} Ctx;

typedef struct {
    u8 pad0[2];
    s16 x2;
    u8 pad4[0x18 - 0x4];
    Vec3f x18;
} Snd;

typedef struct {
    u8 pad0[0xC];
    Snd *xC[(0x100 - 0xC) / 4];
    s32 x100;
} Pool;

typedef struct {
    s16 x0;
    s32 x4;
    u8 x8;
    u8 x9;
} Chan;

extern Vec3f D_800B7F20;
extern Game *gGame;
extern u8 D_8007EE48[];
extern s32 D_80132164[];
extern s32 D_801320C8[];
extern u64 D_80131F90[];
extern Chan D_8007EDF8[];
s16 Obj_NextId(void);
u64 Time_Get(u64 *);
void Pool_PushCopy(Pool *, SndReq *);
Vec3f Car_GetPos(Car *);
void Sound_SetImpactId(SndReq *, s32);
void Sound_SetScrapeId(SndReq *, s32);
void List_RemoveById(Pool *, s16);

void Sound_PlayContact(Pool *pool, Ctx *c) {
    SndReq req;
    Vec3f zero = D_800B7F20;
    s32 slot = -1;
    Car *car = c->x0;
    Car *other = c->x4;
    s32 mode;
    s32 i;
    u64 t;

    if (car->x30 == 0) {
        slot = car->x3C->x10;
    } else if (other != NULL && other->x30 == 0) {
        slot = other->x3C->x10;
    }
    if (slot < 0) {
        return;
    }
    car = c->x0;
    req.x0 = 0x3F4;
    req.x4 = 0;
    req.x2 = Obj_NextId();
    req.x18 = Car_GetPos(car);
    req.x24 = zero;
    req.x44 = 1;
    req.x34 = 0;
    req.x40 = 0;
    req.x45 = 0;
    req.x4C = -1;
    if (c->x14 == 1) {
        req.xC = 6;
    } else {
        req.xC = 5;
    }
    req.x50 = D_8007EE48[req.xC];
    req.x48 = 0;
    if (c->x14 == 1) {
        if (c->x4 != NULL) {
            if (c->x8 != 0) {
                mode = 1;
            } else {
                mode = c->xC != 0;
            }
        } else {
            mode = c->xC;
        }
        Sound_SetImpactId(&req, mode);
    } else {
        if (c->x4 != NULL) {
            mode = 1;
        } else {
            mode = c->xC;
        }
        if (D_8007EDF8[slot].x9 != 0) {
            if (D_8007EDF8[slot].x4 != mode) {
                List_RemoveById(pool, D_8007EDF8[slot].x0);
            } else {
                D_8007EDF8[slot].x8 = 1;
                for (i = 0; i < pool->x100; i++) {
                    if (pool->xC[i]->x2 == D_8007EDF8[slot].x0) {
                        break;
                    }
                }
                pool->xC[i]->x18 = Car_GetPos(car);
                return;
            }
        }
        D_8007EDF8[slot].x9 = 1;
        D_8007EDF8[slot].x8 = 1;
        D_8007EDF8[slot].x4 = mode;
        D_8007EDF8[slot].x0 = req.x2;
        Sound_SetScrapeId(&req, mode);
    }
    if (D_80131F90[req.x10] != -1) {
        t = D_80131F90[req.x10] + Time_Get(&gGame->x1058);
    } else {
        t = -1;
    }
    req.x38 = t;
    req.x30 = D_80132164[req.x10];
    req.x14 = D_801320C8[req.x10] + c->x10;
    if (req.x14 > 256) {
        req.x14 = 256;
    }
    Pool_PushCopy(pool, &req);
}
