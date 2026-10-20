#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    Vec3f a;
    Vec3f b;
} Seg;

typedef struct {
    u8 pad0[0x30];
    Seg *x30;
} Track;

typedef struct {
    u8 pad0[0x1100];
    Track *x1100;
} Game;

typedef struct {
    u8 pad0[0x28];
    s32 x28;
    u8 pad2C[0x88 - 0x2C];
    s32 x88;
} Car;

extern Game *gGame;
extern s32 D_800958B4;
extern s32 D_80095FF8[];
Vec3f Car_GetPos(Car *);
f32 Math_PointLineDist2D(Vec3f, Vec3f, Vec3f);

s32 Car_CompareRacePosition(Car *a, Car *b) {
    s32 sa;
    s32 sb;
    Seg *segs;
    f32 da;

    if (a->x88 > b->x88) {
        return 1;
    }
    if (a->x88 < b->x88) {
        return 0;
    }
    if (a->x88 == D_80095FF8[D_800958B4]) {
        return 1;
    }
    sa = a->x28;
    sb = b->x28;
    if (sa > sb) {
        return 1;
    }
    if (sa < sb) {
        return 0;
    }
    segs = gGame->x1100->x30;
    da = Math_PointLineDist2D(segs[sa].a, segs[sa].b, Car_GetPos(a));
    return da < Math_PointLineDist2D(segs[sb].a, segs[sb].b, Car_GetPos(b));
}
