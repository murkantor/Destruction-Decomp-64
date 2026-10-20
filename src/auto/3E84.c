#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { s32 x0; s32 x4; } Node;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; } Lap;
typedef struct {
    u8 pad[0x30];
    Seg *x30;
    Node *x34;
    s32 x38;
    Lap *x3C;
} Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
typedef struct {
    u8 pad[0x24];
    s32 x24;
    u8 pad28[0x5C - 0x28];
    Vec3f x5C;
    f32 x68;
} Ai;
typedef struct {
    u8 pad[0x28];
    s32 x28;
    u8 pad2C[0x38 - 0x2C];
    Ai *x38;
    u8 pad3C[4];
    u8 x40;
    u8 pad41[3];
    s32 x44;
    u8 pad48[0x58 - 0x48];
    s32 x58;
} Car;
extern Game *gGame;
extern s32 D_8007EAE0[][20];
s32 Racer_WrapNode(Car *, s32);
Vec3f func_8001FC08(Car *);
Vec3f Car_GetPos(Car *);
f32 Vec3f_CrossYSign(Vec3f, Vec3f);

void Ai_UpdateTargetPoint(Car *car) {
    Ai *ai = car->x38;
    Race *race;
    Seg *segs;
    Node *nodes;
    s32 idx;
    Lap *lap;
    f32 f;
    Seg *s;
    Vec3f d;
    Vec3f fwd;
    Vec3f pos;

    ai->x24 = 10;
    race = gGame->race;
    segs = race->x30;
    nodes = race->x34;
    idx = Racer_WrapNode(car, car->x28 - car->x58 * 2);
    if (car->x40) {
        if (idx < race->x3C[car->x44].x4) {
            idx = race->x3C[car->x44].x0;
        }
    } else if (idx < 0) {
        idx += race->x38;
    }
    f = D_8007EAE0[nodes[idx].x4][ai->x24] * 0.01f;
    ai->x5C.x = segs[idx].a.x + (segs[idx].b.x - segs[idx].a.x) * f;
    ai->x5C.y = segs[idx].a.y + (segs[idx].b.y - segs[idx].a.y) * f;
    ai->x5C.z = segs[idx].a.z + (segs[idx].b.z - segs[idx].a.z) * f;
    fwd = func_8001FC08(car);
    pos = Car_GetPos(car);
    d.x = ai->x5C.x - pos.x;
    d.y = ai->x5C.y - pos.y;
    d.z = ai->x5C.z - pos.z;
    ai->x68 = Vec3f_CrossYSign(fwd, d);
}
