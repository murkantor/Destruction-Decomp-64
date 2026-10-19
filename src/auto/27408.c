#include "common.h"
#include "m2c.h"

typedef struct {
    f32 v[4][2][3];
} PosTable;

typedef struct {
    s32 x0;
    f32 x4;
    f32 x8;
    f32 xC;
    f32 x10;
    f32 m[3][3];
    s32 x38;
    u8 pad3C[0x48 - 0x3C];
} Obj48;

typedef struct {
    u8 pad0[0x38];
    Obj48 *x38;
} Owner;

extern PosTable D_800B8DF0;
extern s32 D_800958B4[];
void *Mem_Alloc(u32 size);
void Basis_Orthonormalize(f32 (*m)[3]);

void func_80026808(Owner *owner, s32 idx) {
    PosTable tbl = D_800B8DF0;
    Obj48 *o;
    s32 i;

    o = Mem_Alloc(0x48);
    o->x0 = idx;
    o->x38 = 0;
    o->x4 = 3.0f;
    o->x8 = tbl.v[D_800958B4[0]][idx][0];
    o->xC = tbl.v[D_800958B4[0]][idx][1];
    o->x10 = tbl.v[D_800958B4[0]][idx][2];
    for (i = 0; i < 3; i++) {
        if (i == 0) o->m[i][0] = 1.0f; else o->m[i][0] = 0.0f;
        if (i == 1) o->m[i][1] = 1.0f; else o->m[i][1] = 0.0f;
        if (i == 2) o->m[i][2] = 1.0f; else o->m[i][2] = 0.0f;
    }
    Basis_Orthonormalize(o->m);
    owner->x38 = o;
}
