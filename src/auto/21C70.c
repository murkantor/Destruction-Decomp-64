#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 v[8]; } F8;
typedef struct { s16 x0; } Src;
typedef struct {
    s16 x0; s16 x2; void *x4; u8 pad8[4]; u8 xC[8]; f32 x14; f32 x18; f32 x1C; u8 pad20[4]; f32 x24;
    u8 pad28[0x40]; Vec3f x68; Vec3f x74; Vec3f x80; Vec3f x8C; Vec3f x98; Vec3f xA4; Src *xB0; u8 xB4; u8 xB5; u8 xB6; u8 xB7; f32 xB8;
} Obj;
typedef struct { u8 pad0[0x10FC]; s16 x10FC; } Game;
extern F8 D_800B8AD0;
extern F8 D_800B8AF0;
extern u32 D_800958B4[];
extern s8 D_800D31CC;
extern f32 D_80091C7C[];
extern f32 D_80095610[];
extern u8 D_80095628;
extern Game *gGame[];
void *Mem_Alloc(u32);
s16 func_80064D70(void);
void func_8005B220(void *, s32);
void Vec3f_Set(Vec3f *, f32, f32, f32);
void func_80023CE0(Obj *);

Obj *Obj_Create(Src *src) {
    F8 a;
    F8 b;
    Obj *o;

    a = D_800B8AD0;
    b = D_800B8AF0;
    o = Mem_Alloc(sizeof(Obj));
    if (o != NULL) {
        o->x0 = 9;
        o->x2 = func_80064D70();
        o->x4 = o;
        func_8005B220(o->xC, 2);
        o->x24 = 6.5f;
        if (D_800958B4[0] >= 4) {
            o->x14 = b.v[D_800958B4[0] - 4];
            o->x18 = a.v[D_800958B4[0] - 4];
        } else {
            o->x14 = 1.0f;
            o->x18 = 550.0f;
        }
        o->x1C = D_80091C7C[D_800D31CC];
        Vec3f_Set(&o->x68, 0.0f, 1.0f, 0.0f);
        o->xB0 = src;
        switch (src->x0) {
        case 1:
            o->xB4 = 2;
            break;
        case 2:
            o->xB4 = 5;
            break;
        }
        Vec3f_Set(&o->x74, 1.0f, 0.0f, 0.0f);
        Vec3f_Set(&o->x80, 0.0f, 1.0f, 0.0f);
        Vec3f_Set(&o->x8C, 0.0f, 0.0f, 1.0f);
        Vec3f_Set(&o->x98, 0.0f, 0.0f, 0.0f);
        Vec3f_Set(&o->xA4, 0.0f, 0.0f, 0.0f);
        o->xB6 = 0;
        o->xB8 = 0.0f;
        if (gGame[0]->x10FC == 0x19) {
            func_80023CE0(o);
        } else {
            D_80095628 = 0;
        }
        D_80095610[D_800D31CC] = -1.5707964f;
    }
    return o;
}
