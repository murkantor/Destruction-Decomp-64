#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[4];
    s32 x4;
    f32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    f32 x18;
    f32 x1C;
    s32 x20;
    s32 x24;
    s32 x28;
    s32 x2C;
    s32 x30;
    s32 x34;
    s32 x38;
} Cfg;
typedef struct Obj {
    s16 x0;
    s16 x2;
    struct Obj *x4;
    u8 pad8[4];
    s32 xC;
    s32 x10;
    s32 x14;
    u8 pad18[4];
    s32 x1C;
    f32 x20;
    s32 x24;
    s32 x28;
    s32 x2C;
    s32 x30;
    f32 x34;
    f32 x38;
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
    s32 x4C;
    s32 x50;
    s32 x54;
    s32 x58;
    s32 x5C;
    s32 x60;
    s32 x64;
    s32 x68;
    u8 x6C;
    u8 x6D;
    u8 pad6E[2];
    s64 x70;
    s32 x78;
    u8 pad7C[4];
} Obj;
extern s32 D_800958A8[];
extern s32 D_800958AC;
void *Mem_Alloc(s32);
s16 func_80064D70(void);

Obj *Obj_CreateFromCfg(Cfg *c) {
    Obj *o = Mem_Alloc(sizeof(Obj));

    if (o != NULL) {
        o->x0 = 7;
        o->x2 = func_80064D70();
        o->x4 = o;
        o->xC = c->x4;
        o->x1C = c->x34;
        o->x20 = c->x8;
        o->x24 = c->xC;
        o->x2C = c->x10;
        o->x30 = c->x14;
        o->x38 = c->x1C;
        o->x3C = c->x20;
        if (D_800958A8[0] == 3 && D_800958AC == 1) {
            o->x40 = D_800958AC;
            o->x34 = 500.0f;
        } else {
            o->x40 = c->x24;
            o->x34 = c->x18;
        }
        o->x44 = c->x28;
        o->x48 = c->x2C;
        o->x4C = c->x30;
        o->x50 = c->x34;
        o->x54 = c->x38;
        o->x58 = c->x38;
        o->x70 = 0;
        o->x5C = 0;
        o->x60 = 0;
        o->x64 = 0;
        o->x68 = 0;
        o->x28 = 0;
        o->x10 = 0;
        o->x14 = 0;
        o->x6C = 0;
        o->x6D = 0;
        o->x78 = 0;
        return o;
    }
    return NULL;
}
