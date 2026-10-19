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
extern u64 gGameTime;
extern u8 D_8007EE48[];
extern s32 D_80132164[];
extern s32 D_801320C8[];
extern u64 D_80131F90[];
s16 func_80064D70(void);
u64 Time_Get(u64 *);
void Pool_PushCopy(void *, SndReq *);

void Sound_PlayAt(void *pool, Vec3f pos, Vec3f vel, s32 kind) {
    SndReq req;
    u64 *timer = &gGame->x1058;
    u64 t;

    req.x0 = 0x3F4;
    req.x4 = 0;
    req.x2 = func_80064D70();
    req.xC = 4;
    req.x18 = pos;
    req.x24 = vel;
    req.x44 = 1;
    req.x34 = 0;
    req.x40 = 0;
    req.x45 = 0;
    req.x4C = -1;
    req.x50 = D_8007EE48[req.xC];
    req.x48 = 0;
    switch (kind) {
        case 0:
            req.x10 = 0x18;
            req.x34 = 4.0f;
            break;
        case 1:
            req.x10 = 0x19;
            break;
        case 2:
            req.x10 = 0x1A;
            break;
        case 3:
            req.x10 = 0x1B;
            break;
        case 4:
            req.x10 = 0x20;
            break;
        case 9:
            req.x10 = 0x21;
            break;
        case 10:
            req.x10 = 0x22;
            break;
        case 5:
            timer = &gGameTime;
            req.x10 = 0x26;
            req.xC = 0;
            req.x50 = D_8007EE48[0];
            break;
        case 6:
            timer = &gGameTime;
            req.x10 = 0x24;
            req.xC = 0;
            req.x50 = D_8007EE48[0];
            break;
        case 7:
            timer = &gGameTime;
            req.x10 = 0x25;
            req.xC = 0;
            req.x50 = D_8007EE48[0];
            break;
        case 8:
            timer = &gGameTime;
            req.x10 = 0x23;
            req.xC = 0;
            req.x50 = D_8007EE48[0];
            break;
    }
    req.x30 = D_80132164[req.x10];
    req.x14 = D_801320C8[req.x10];
    if (D_80131F90[req.x10] != -1) {
        t = D_80131F90[req.x10] + Time_Get(timer);
    } else {
        t = -1;
    }
    req.x38 = t;
    Pool_PushCopy(pool, &req);
}

const s32 D_800B7F8C[5] = { 0, 0, 0, 0, 0 };
