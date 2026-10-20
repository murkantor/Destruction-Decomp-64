#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[0x20]; s32 x24; u8 pad28[0x18]; } Ent;
typedef struct {
    u8 pad[0x24];
    u8 x24;
    u8 pad25[3];
    Ent *x28;
    u8 pad2C[0x50 - 0x2C];
    void *x50;
    void *x54;
    void *x58;
    void *x5C;
    void *x60;
} Obj;
void *func_80063E0C(void *, s32, s32);
void Surface_InitTable(s32);
void Surface_AddTexture(s32);
s32 Res_Get(s32);

void func_80024BA0(Obj *o, s32 a, s32 b) {
    s32 i;

    o->x28 = func_80063E0C(o->x28, a, b);
    o->x50 = func_80063E0C(o->x50, a, b);
    o->x54 = func_80063E0C(o->x54, a, b);
    o->x58 = func_80063E0C(o->x58, a, b);
    o->x5C = func_80063E0C(o->x5C, a, b);
    o->x60 = func_80063E0C(o->x60, a, b);
    Surface_InitTable(o->x24);
    for (i = 0; i < o->x24; i++) {
        Surface_AddTexture(o->x28[i].x0);
        o->x28[i].x0 = Res_Get(o->x28[i].x0);
        if (o->x28[i].x24 != 0) {
            o->x28[i].x24 = Res_Get(o->x28[i].x24);
        } else {
            o->x28[i].x24 = 0;
        }
    }
}
