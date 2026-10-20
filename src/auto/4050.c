#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { u8 pad[0x30]; Seg *x30; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct { u8 pad[0x28]; s32 x28; u8 pad2C[0x58 - 0x2C]; s32 x58; } Car;
extern Game *gGame;
Vec3f Car_GetPos(Car *);
void func_8001FC08(Vec3f *, Car *);
f32 Vec3f_AngleBetween(Vec3f, Vec3f);
s32 Racer_WrapNode(Car *, s32);
int abs(int);

s32 func_80003450(Car *car)
{
    Vec3f b1, a1, b2, a2, fwd, pos;
    s32 idx;
    Seg *segs;
    Seg *s;
    f32 angA, angB, newA, newB;

    idx = car->x28;
    segs = gGame->x1100->x30;
    pos = Car_GetPos(car);
    func_8001FC08(&fwd, car);
    s = (Seg *)(idx * sizeof(Seg) + (u32)segs);
    a1.x = s->a.x - pos.x;
    a1.y = s->a.y - pos.y;
    a1.z = s->a.z - pos.z;
    b1.x = s->b.x - pos.x;
    b1.y = s->b.y - pos.y;
    b1.z = s->b.z - pos.z;
    angA = Vec3f_AngleBetween(fwd, a1);
    angB = Vec3f_AngleBetween(fwd, b1);
    idx = Racer_WrapNode(car, idx + car->x58);
    s = (Seg *)(idx * sizeof(Seg) + (u32)segs);
    a2.x = s->a.x - pos.x;
    a2.y = s->a.y - pos.y;
    a2.z = s->a.z - pos.z;
    b2.x = s->b.x - pos.x;
    b2.y = s->b.y - pos.y;
    b2.z = s->b.z - pos.z;
    newA = Vec3f_AngleBetween(fwd, a2);
    newB = Vec3f_AngleBetween(fwd, b2);
    while ((newA < angA) & (newB < angB)) {
        idx = Racer_WrapNode(car, idx + car->x58);
        s = (Seg *)(idx * sizeof(Seg) + (u32)segs);
        a2.x = s->a.x - pos.x;
        a2.y = s->a.y - pos.y;
        a2.z = s->a.z - pos.z;
        b2.x = s->b.x - pos.x;
        b2.y = s->b.y - pos.y;
        b2.z = s->b.z - pos.z;
        angA = newA;
        newA = Vec3f_AngleBetween(fwd, a2);
        angB = newB;
        newB = Vec3f_AngleBetween(fwd, b2);
    }
    if (abs(idx - car->x28) < 2) {
        idx = Racer_WrapNode(car, idx + car->x58 * 2);
    }
    return idx;
}
