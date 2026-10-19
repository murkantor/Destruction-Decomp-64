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
    f32 x34;
    u64 x38;
    s32 x40;
    u8 x44;
    u8 x45;
    s32 x48;
    s32 x4C;
    u8 x50;
} SndReq;
extern Game *gGame;
extern u8 D_8007EE48[];
extern s32 D_80132164[];
extern s32 D_801320C8[];
extern u64 D_80131F90[];
s16 func_80064D70(void);
u64 Time_Get(u64 *);
s8 Pool_PushCopy(void *, SndReq *);

s16 func_800072CC(void *pool, Vec3f *a, Vec3f *b, s32 type, f32 vol, s32 mode) {
    SndReq req;
    u64 t;

    req.x0 = 0x3F4;
    req.x4 = 0;
    req.x2 = func_80064D70();
    req.xC = (mode == 2) ? 8 : 7;
    req.x18 = *a;
    req.x24 = *b;
    req.x34 = 0.0f;
    req.x40 = 0;
    req.x44 = 1;
    req.x45 = 0;
    req.x4C = -1;
    req.x50 = D_8007EE48[req.xC];
    req.x48 = 0;
    switch (type) {
        case 1:
            req.x10 = 4;
            break;
        case 2:
            req.x10 = 9;
            break;
        case 3:
            req.x10 = 6;
            break;
        case 4:
            req.x10 = 0xA;
            break;
        case 5:
            req.x10 = 0x16;
            break;
        case 6:
            req.x10 = 0x14;
            break;
        case 7:
            req.x10 = 5;
            break;
        case 8:
            req.x10 = 5;
            break;
        case 9:
            req.x10 = 7;
            break;
        case 0:
        case 10:
        default:
            req.x10 = 0x1C;
            break;
    }
    req.x14 = D_801320C8[req.x10] * vol;
    req.x30 = D_80132164[req.x10];
    if (D_80131F90[req.x10] != -1) {
        t = D_80131F90[req.x10] + Time_Get(&gGame->x1058);
    } else {
        t = -1;
    }
    req.x38 = t;
    if (!Pool_PushCopy(pool, &req)) {
        req.x2 = 0;
    }
    return req.x2;
}
