#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { Vec3f c; f32 r; } Circle;
typedef struct { s32 x0; s32 x4; } Elem8;
typedef struct {
    u8 pad0[0xC];
    s32 xC;
    Circle *x10;
    s32 x14;
    Elem8 *x18;
} TrkData;
typedef struct { u8 pad0[0x2C]; TrkData *x2C; } Trk;
typedef struct { u8 pad0[0x1100]; Trk *x1100; } Game;
typedef struct {
    u8 pad0[0x18];
    void *x18;
    u8 pad1C[0x5C - 0x1C];
    Vec3f x5C;
    f32 x68;
} Obj;
typedef struct { u8 pad0[0x38]; Obj *x38; } Car;

extern Game *gGame;
Vec3f func_8001FBA8(void *);
Vec3f func_8001FC08(Car *);
Vec3f func_80003EE8(Seg *, Elem8 *);
Vec3f Seg_AvoidCircleXZ(Seg *, Circle *);
u8 Track_IsGridCellSolid(Vec3f *);
f32 func_800005C8(Vec3f, Vec3f);

void func_80003780(Car *car) {
    Obj *o;
    Trk *trk;
    Seg seg;
    s32 pad[2];
    Vec3f orig;
    Vec3f b;
    Vec3f a;
    Vec3f d;
    s32 i;
    u8 changed;

    o = car->x38;
    trk = gGame->x1100;
    orig = o->x5C;
    seg.a = func_8001FBA8(o->x18);
    seg.b = o->x5C;
    changed = 0;
    for (i = 0; i < trk->x2C->x14; i++) {
        o->x5C = func_80003EE8(&seg, &trk->x2C->x18[i]);
        seg.b = o->x5C;
    }
    if (orig.x != o->x5C.x || orig.y != o->x5C.y || orig.z != o->x5C.z) {
        changed = 1;
    }
    for (i = 0; i < trk->x2C->xC; i++) {
        o->x5C = Seg_AvoidCircleXZ(&seg, &trk->x2C->x10[i]);
        seg.b = o->x5C;
    }
    if (orig.x != o->x5C.x || orig.y != o->x5C.y || orig.z != o->x5C.z) {
        changed = 1;
    }
    seg.a = func_8001FBA8(car);
    seg.b = o->x5C;
    for (i = 0; i < trk->x2C->x14; i++) {
        o->x5C = func_80003EE8(&seg, &trk->x2C->x18[i]);
        seg.b = o->x5C;
    }
    if (orig.x != o->x5C.x || orig.y != o->x5C.y || orig.z != o->x5C.z) {
        changed = 1;
    }
    for (i = 0; i < trk->x2C->xC; i++) {
        o->x5C = Seg_AvoidCircleXZ(&seg, &trk->x2C->x10[i]);
        seg.b = o->x5C;
    }
    if (orig.x != o->x5C.x || orig.y != o->x5C.y || orig.z != o->x5C.z) {
        changed = 1;
    }
    if (!Track_IsGridCellSolid(&o->x5C)) {
        o->x5C = func_8001FBA8(o->x18);
        changed = 1;
    }
    if (changed) {
        a = func_8001FC08(car);
        b = func_8001FBA8(car);
        d.x = o->x5C.x - b.x;
        d.y = o->x5C.y - b.y;
        d.z = o->x5C.z - b.z;
        o->x68 = func_800005C8(a, d);
    }
}
