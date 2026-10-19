#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1C]; f32 x1C; u8 pad20[0x94]; u8 xB4; } Obj;
typedef struct { s32 x0; s32 x4; s32 x8; f32 xC; } CamState;
extern CamState D_80130200;
extern s32 D_800958B4;
extern s32 D_800955A4[];
extern u8 D_80095628;
extern s32 D_80123320;
extern f32 D_80091C80;
extern void *D_8009562C;

void func_80023CE0(Obj *o) {
    D_80130200.x4 = -1;
    D_80130200.x8 = 1;
    D_80130200.x0 = 0;
    D_80130200.xC = 10.0f;
    D_80095628 = 1;
    D_80123320 = D_800955A4[D_800958B4];
    o->xB4 = 2;
    o->x1C = D_80091C80;
    D_8009562C = NULL;
}
