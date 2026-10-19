#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct Spark {
    f32 x0;
    f32 x4;
    f32 x8;
    f32 xC;
    f32 x10;
    f32 x14;
    f32 x18;
    s32 x1C;
    f32 x20;
    s32 x24;
    u8 x28[0x50 - 0x28];
    struct Spark *x50;
} Spark;

typedef struct {
    u8 pad0[0x34];
    s32 x34;
    u8 pad38[0x178 - 0x38];
    u8 x178[0x23C - 0x178];
    Vec3f x23C;
    u8 pad248[0x290 - 0x248];
    f32 x290;
    f32 x294;
    f32 x298;
} Car;

extern u32 D_80125C58;
extern s32 D_801311B8;
extern Spark *D_80125B70[];
void *Mem_Alloc(u32 size);
void func_8005AA68(void *, void *);
void func_8005AB4C(void *, f32);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);

void Car_SpawnSpark(Car *car, s8 side) {
    s32 idx = car->x34;
    Spark *p;
    Vec3f v;
    void *q;

    if (D_80125C58 <= 0x2000) {
        return;
    }
    p = Mem_Alloc(0x54);
    if (p == NULL) {
        return;
    }
    q = p->x28;
    D_801311B8++;
    p->x50 = D_80125B70[(s8)idx];
    D_80125B70[(s8)idx] = p;
    func_8005AA68(q, car->x178);
    func_8005AB4C(q, 0.01f);
    v = car->x23C;
    if (!side) {
        Vec3f_Scale(&v, &v, 1.5f);
    } else {
        Vec3f_Scale(&v, &v, -1.5f);
    }
    p->x4 = car->x290 + v.x;
    p->x8 = car->x294 + v.y + 1.0;
    p->xC = car->x298 + v.z;
    p->x10 = 0.05f;
    p->x14 = 0.04f;
    p->x18 = 0.03f;
    p->x1C = 0;
    p->x0 = p->x8;
    p->x20 = 10.0f;
    p->x24 = 0;
}
