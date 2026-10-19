#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r[3]; } Mtx3;
typedef struct { Vec3f *p[4]; } Tbl4;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x10]; Gfx *x10; u8 pad14[0xC]; s32 x20; } ResObj;
typedef struct Car {
    s16 x0;
    s16 x2;
    struct Car *x4;
    u8 pad8[4];
    u8 xC;
    u8 xD;
    u8 padE[0x22];
    s32 x30;
    s32 x34;
    u8 pad38[0x290 - 0x38];
    Vec3f x290;
    u8 pad29C[0x2B4 - 0x29C];
    Mtx3 x2B4;
    u8 pad2D8[0xB80 - 0x2D8];
    s32 xB80;
    u8 padB84[0xEA0 - 0xB84];
} Car;
typedef struct Obj14 {
    s16 x0;
    s16 x2;
    struct Obj14 *x4;
    u8 pad8[4];
    Car *xC;
    s8 x10;
} Obj14;
typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[0x70];
    u8 x78[0x2B0 - 0x78];
} ResBuf;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;

extern s32 D_80091AB4[];
extern s32 D_800919F4[];
extern Game *gGame;
extern u8 D_801253F8;
extern u8 D_80097A0C[];
extern u8 D_80124360;
extern u8 D_80125C68[];
extern s32 D_800958A4;
extern Tbl4 D_800BA360;
extern Mtx3 D_801242C0[];

void *Mem_Alloc(s32);
s16 func_80064D70(void);
void Res_Load(s32, void *);
s32 Res_Get(s32);
ResObj *func_80063E28(s32);
void RigidBody_Init(void *, void *);
void func_8005F9C0(void *, void *, void *, s32, s32, s32, f32);
void func_8005A970(void *, s32, s32, s32);
void func_8005AA68(void *, void *);
void func_8005AB4C(void *, f32);
void Mtx3_RotateY(Mtx3 *, f32);
void func_80064100(Obj14 *);

Obj14 *func_8003E644(s32 id, s32 slot) {
    ResBuf buf;
    s32 col[2];
    Obj14 *obj;
    Car *car;
    ResObj *res;
    s32 r, g, b;
    s32 i;
    u8 flag;
    Gfx *gfx;

    obj = Mem_Alloc(0x14);
    obj->x0 = 0x18;
    obj->x2 = func_80064D70();
    obj->x4 = obj;
    obj->x10 = slot;
    Res_Load(D_80091AB4[id], &buf);
    car = Mem_Alloc(0xEA0);
    obj->xC = car;
    car->x0 = 3;
    car->x2 = func_80064D70();
    car->x4 = car;
    car->xC = 1;
    car->xD = 0;
    car->x34 = buf.x4;
    car->x30 = 1;
    car->xB80 = 0;
    RigidBody_Init((u8 *)car + 0x1EC, buf.x78);
    res = func_80063E28(Res_Get(D_800919F4[car->x34]));
    r = 0xFA;
    if (gGame->x10FC == 24) {
        if (id != -1) {
            if ((D_801253F8 != 0) | (id < 12)) {
                g = 0xFA;
                b = 0xFA;
            } else if (id < 12 || D_80097A0C[id] == 0) {
                r = 0;
                g = 0;
                b = 0;
            } else {
                g = 0xFA;
                b = 0xFA;
            }
        } else {
            g = 0xFA;
            b = 0xFA;
        }
    } else {
        g = 0xFA;
        b = 0xFA;
    }
    gfx = res->x10;
    gfx->w0 = 0xFA000000;
    gfx->w1 = (r << 24) | (g << 16) | (b << 8);
    func_8005F9C0(col, (u8 *)car + 0x160, (u8 *)car + 0x98, res->x20, 0, 0, 0.01f);
    if (!D_80124360) {
        D_80124360 = 1;
        func_8005A970(D_80125C68, func_80063E28(Res_Get(0x4650))->x20, 0, 0);
    }
    for (i = 0; i < 4; i++) {
        func_8005AA68((u8 *)car + 0xC0 + i * 0x28, D_80125C68);
        func_8005AB4C((u8 *)car + 0xC0 + i * 0x28, 0.01f);
    }
    {
        Tbl4 tbl = D_800BA360;
        Vec3f *v = tbl.p[D_800958A4 - 1];
        Car *c = obj->xC;
        v += slot;
        c->x290.x = v->x;
        c->x290.y = v->y;
        c->x290.z = v->z;
        c->x2B4.r[0] = D_801242C0[obj->x10].r[0];
        c->x2B4.r[1] = D_801242C0[obj->x10].r[1];
        c->x2B4.r[2] = D_801242C0[obj->x10].r[2];
        Mtx3_RotateY(&c->x2B4, 0.09773844f);
    }
    func_80064100(obj);
    return obj;
}
