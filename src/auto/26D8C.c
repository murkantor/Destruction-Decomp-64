#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    f32 xC;
    u8 pad10[0x1C - 0x10];
    Vec3f m[3];
    u64 x40;
    u8 x48;
    u8 x49;
    u8 x4A;
    u8 pad4B[0x50 - 0x4B];
} UnkObj;
typedef struct { u8 pad[0x38]; UnkObj *x38; } UnkParent;
void *Mem_Alloc(u32 size);
u64 Time_Get(u64 *);
void func_800266C8(UnkObj *);
void Basis_Orthonormalize(Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern Game *gGame[];

void Obj_CreateWithBasis(UnkParent *p, s32 a) {
    UnkObj *o = Mem_Alloc(0x50);
    o->x0 = a;
    o->x4 = 0;
    o->x8 = 0;
    o->x49 = 0;
    o->x4A = 0;
    o->x40 = Time_Get(&gGame[0]->x10A8);
    o->x48 = 60;
    o->xC = 1.5f;
    func_800266C8(o);
    o->m[0].x = 1.0f;
    o->m[0].y = 0.0f;
    o->m[0].z = 0.0f;
    o->m[1].x = 0.0f;
    o->m[1].y = 1.0f;
    o->m[1].z = 0.0f;
    o->m[2].x = 0.0f;
    o->m[2].y = 0.0f;
    o->m[2].z = 1.0f;
    Basis_Orthonormalize(o->m);
    Vec3f_Scale(&o->m[0], &o->m[0], 2.0f);
    Vec3f_Scale(&o->m[1], &o->m[1], 2.0f);
    Vec3f_Scale(&o->m[2], &o->m[2], 2.0f);
    p->x38 = o;
}
