#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { u8 pad[0x30]; Seg *x30; u8 pad2[4]; s32 x38; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
extern Game *gGame;
f32 Math_PointLineDist2D(Vec3f, Vec3f, Vec3f);
f32 Vec3f_DistXZ(Vec3f, Vec3f);

s32 Track_FindNearestSegment(Vec3f pos) {
    Track *t = gGame->x1100;
    s32 best = -1;
    f32 bestDist = 100.0f;
    s32 i;
    s32 d;
    s32 dxz;

    for (i = 0; i < t->x38; i++) {
        d = Math_PointLineDist2D(pos, t->x30[i].a, t->x30[i].b);
        dxz = Vec3f_DistXZ(pos, t->x30[i].a);
        if ((f32)d < bestDist && dxz < 50) {
            bestDist = d;
            best = i;
        }
    }
    return best;
}
