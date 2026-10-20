#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    u16 x0;
    u8 pad2[2];
    s32 x4;
    Vec3f x8;
    Vec3f x14;
} Spawn;

typedef struct {
    u8 pad0[0x20];
    s32 x20;
} Res;

typedef struct {
    u8 pad0[0x18];
} Anim;

typedef struct {
    u16 x0;
    u8 pad2[2];
    s32 x4;
    Res *x8;
    Anim *xC;
    u8 x10[0x3C - 0x10];
} Obj;

extern Res *D_80125578[];
extern Anim D_800D1190[];
extern s32 D_8011F048[];
void *Mem_AllocAligned(u32, s32, s32);
s32 Res_Get(s32);
Res *Res_GetEnd(s32);
void Model_Init(Anim *, s32, s32, s32);
void ModelInst_Init(void *, Anim *);
void ModelInst_SetScale(void *, f32);
void Prop_Create(Obj *, Vec3f);
void Prop_CreateStatic(Obj *, Vec3f);
void Mover_Create(Obj *, Vec3f, Vec3f);
void Timer_Create(Obj *);
void Obj_CreateWithBasis(Obj *, s32);
void func_80026808(Obj *, s32);
void func_80025FCC(Obj *, Vec3f, Vec3f);
void func_800260EC(Obj *, Vec3f, Vec3f);

Obj *Obj_CreateFromSpawn(Spawn *sp) {
    Obj *obj = Mem_AllocAligned(0x3C, 7, 0);
    s32 kind;

    switch ((s16)sp->x0) {
    case 23:
        kind = 0;
        func_80025FCC(obj, sp->x8, sp->x14);
        break;
    case 6:
        kind = 1;
        Prop_Create(obj, sp->x8);
        break;
    case 19:
        kind = 2;
        Prop_CreateStatic(obj, sp->x8);
        break;
    case 27:
        kind = 3;
        Mover_Create(obj, sp->x8, sp->x14);
        break;
    case 28:
        kind = 4;
        Mover_Create(obj, sp->x8, sp->x14);
        break;
    case 14:
        Timer_Create(obj);
        kind = 5;
        break;
    case 15:
        Obj_CreateWithBasis(obj, 1);
        kind = 6;
        break;
    case 16:
        func_80026808(obj, 1);
        kind = 7;
        break;
    case 17:
        Obj_CreateWithBasis(obj, 0);
        kind = 8;
        break;
    case 18:
        func_80026808(obj, 0);
        kind = 9;
        break;
    case 29:
        kind = 10;
        Mover_Create(obj, sp->x8, sp->x14);
        break;
    case 30:
        kind = 11;
        func_800260EC(obj, sp->x8, sp->x14);
        break;
    case 33:
        kind = 12;
        func_80025FCC(obj, sp->x8, sp->x14);
        break;
    case 32:
        kind = 13;
        func_80025FCC(obj, sp->x8, sp->x14);
        break;
    default:
        return NULL;
    }
    if (D_80125578[kind] == NULL) {
        D_80125578[kind] = Res_GetEnd(Res_Get(sp->x4));
        Model_Init(&D_800D1190[kind], D_80125578[kind]->x20, 0, 0);
    }
    obj->xC = &D_800D1190[kind];
    obj->x8 = D_80125578[kind];
    obj->x4 = sp->x4;
    D_8011F048[kind]++;
    ModelInst_Init(obj->x10, obj->xC);
    ModelInst_SetScale(obj->x10, 0.1f);
    obj->x0 = sp->x0;
    return obj;
}
