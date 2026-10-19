#include "common.h"
#include "m2c.h"

typedef struct { f32 v[4][2][3]; } Tbl;
extern Tbl D_800B8D90;
extern s32 D_800958B4[];
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; f32 x10, x14, x18; } Obj;
void func_800266C8(Obj *o) {
    Tbl t = D_800B8D90;
    o->x4 = 0;
    o->x8 = 0;
    o->x10 = t.v[D_800958B4[0]][o->x0][0];
    o->x14 = t.v[D_800958B4[0]][o->x0][1];
    o->x18 = t.v[D_800958B4[0]][o->x0][2];
}
