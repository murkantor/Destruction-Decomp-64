#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { s32 x0; s32 x4; } SegInfo;
typedef struct { u8 pad[0x30]; Seg *x30; SegInfo *x34; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct Obj { u8 pad[0x18]; struct Obj *x18; u8 pad1C[0x24 - 0x1C]; s32 x24; s32 x28; u8 pad2C[0x38 - 0x2C]; struct Obj *x38; } Obj;
extern Game *gGame;
extern s32 D_8007EAE0[][20];
Vec3f Car_GetPos(Obj *);
f32 Vec3f_DistXZ(Vec3f, Vec3f);
f32 sqrtf(f32);

void Car_UpdateTrackSection(Obj *obj) {
    Obj *o = obj->x38;
    s32 idx = obj->x28;
    Seg *segs = gGame->x1100->x30;
    SegInfo *info = gGame->x1100->x34;
    Vec3f dir, a, b, pos;
    f32 d1, d2, d3, s, t, len, r;
    s32 i, j;

    a = segs[idx].a;
    b = segs[idx].b;
    pos = Car_GetPos(obj);
    d1 = Vec3f_DistXZ(a, pos);
    d2 = Vec3f_DistXZ(b, pos);
    d3 = Vec3f_DistXZ(a, b);
    s = (d2 * d2 - d1 * d1 + d3 * d3) / (d3 + d3);

    j = o->x18->x28;
    a = segs[j].a;
    b = segs[j].b;
    pos = Car_GetPos(o->x18);
    d1 = Vec3f_DistXZ(a, pos);
    d2 = Vec3f_DistXZ(b, pos);
    d3 = Vec3f_DistXZ(a, b);
    dir.x = b.x - a.x;
    dir.y = 0.0f;
    dir.z = b.z - a.z;
    len = sqrtf(dir.x * dir.x + dir.z * dir.z);
    t = (d2 * d2 - d1 * d1 + d3 * d3) / (d3 + d3);
    dir.x /= len;
    dir.z /= len;

    if (s <= t) {
        if (t < 5.0f) {
            r = (t + 5.0f) / len * 100.0f;
        } else {
            r = (t - 5.0f) / len * 100.0f;
        }
    } else {
        if (len - t < 5.0f) {
            r = (t - 5.0f) / len * 100.0f;
        } else {
            r = (t + 5.0f) / len * 100.0f;
        }
    }

    for (i = 0; i < 20; i++) {
        if (r <= D_8007EAE0[info[idx].x4][i] * 0.01f) {
            o->x24 = i;
            break;
        }
    }
}
