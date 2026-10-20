#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { s32 x0; s32 x4; } Elem8;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; } Elem16;
typedef struct {
    u8 pad0[0x30];
    Seg *x30;
    Elem8 *x34;
    u8 pad38[4];
    Elem16 *x3C;
    s32 x40;
} Trk;
typedef struct { u8 pad0[0x1100]; Trk *x1100; } Game;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0xC];
    f32 x20;
    s32 x24;
    s32 x28;
    u8 pad2C[0x5C - 0x2C];
    Vec3f x5C;
    f32 x68;
} Obj;
typedef struct {
    u8 pad0[0x28];
    s32 x28;
    u8 pad2C[0xC];
    Obj *x38;
    u8 pad3C[4];
    u8 x40;
    u8 pad41[3];
    s32 x44;
} Car;

extern Game *gGame;
extern s32 D_8007E950[];
extern s32 D_8007EAE0[][20];
s32 rand(void);
s32 func_80003450(Car *);
Vec3f Car_GetVelocity(Car *);
Vec3f Car_GetPos(Car *);
Vec3f func_8001FC08(Car *);
f32 Vec3f_AngleBetween(Vec3f, Vec3f);
f32 func_800005C8(Vec3f, Vec3f);

void Ai_UpdateLaneOffset(Car *car) {
    Vec3f t;
    Vec3f fwd;
    Vec3f pos;
    Obj *o;
    Trk *trk;
    Seg *segs;
    Elem8 *e8;
    s32 rnd;
    f32 f;
    s32 i;
    f32 a1;
    f32 a2;
    f32 s;
    s32 ten;

    o = car->x38;
    ten = 10;
    { s32 r = rand() % 100; rnd = D_8007E950[r]; }
    if (o->x10 == 8 || o->x10 == 9) {
        f = 0.0f;
    } else {
        f = (ten - o->x24) * o->x20;
    }
    if (o->x20 != 0.0f) {
        if (rnd < 0) {
            if (f > 0.0f) {
                f = -f;
            }
        } else {
            if (f < 0.0f) {
                f = -f;
            }
        }
        o->x24 += (s32)f + rnd;
    } else if (!(o->x10 == 8 || o->x10 == 9)) {
        o->x24 = ten;
    }
    if (o->x10 == 12) {
        o->x24 = ten;
    }
    if (o->x24 >= 20) {
        o->x24 = 19;
    }
    if (o->x24 < 0) {
        o->x24 = 0;
    }
    trk = gGame->x1100;
    segs = trk->x30;
    e8 = trk->x34;
    o->x28 = func_80003450(car);
    if (!car->x40) {
        for (i = 0; i < trk->x40; i++) {
            if (car->x28 == trk->x3C[i].x0) {
                fwd = Car_GetVelocity(car);
                pos = Car_GetPos(car);
                t.x = segs[o->x28].a.x + (segs[o->x28].b.x - segs[o->x28].a.x) * 0.5;
                t.y = segs[o->x28].a.y + (segs[o->x28].b.y - segs[o->x28].a.y) * 0.5;
                t.z = segs[o->x28].a.z + (segs[o->x28].b.z - segs[o->x28].a.z) * 0.5;
                t.x -= pos.x;
                t.y -= pos.y;
                t.z -= pos.z;
                a1 = Vec3f_AngleBetween(fwd, t);
                t.x = segs[trk->x3C[i].x4].a.x + (segs[trk->x3C[i].x4].b.x - segs[trk->x3C[i].x4].a.x) * 0.5;
                t.y = segs[trk->x3C[i].x4].a.y + (segs[trk->x3C[i].x4].b.y - segs[trk->x3C[i].x4].a.y) * 0.5;
                t.z = segs[trk->x3C[i].x4].a.z + (segs[trk->x3C[i].x4].b.z - segs[trk->x3C[i].x4].a.z) * 0.5;
                t.x -= pos.x;
                t.y -= pos.y;
                t.z -= pos.z;
                a2 = Vec3f_AngleBetween(fwd, t);
                if (a2 < a1) {
                    car->x28 = o->x28 = trk->x3C[i].x4;
                    car->x40 = 1;
                    car->x44 = i;
                }
                break;
            }
        }
    }
    s = D_8007EAE0[e8[o->x28].x4][o->x24] * 0.01f;
    o->x5C.x = segs[o->x28].a.x + (segs[o->x28].b.x - segs[o->x28].a.x) * s;
    o->x5C.y = segs[o->x28].a.y + (segs[o->x28].b.y - segs[o->x28].a.y) * s;
    o->x5C.z = segs[o->x28].a.z + (segs[o->x28].b.z - segs[o->x28].a.z) * s;
    fwd = func_8001FC08(car);
    fwd.y = 0.0f;
    pos = Car_GetPos(car);
    t.x = o->x5C.x - pos.x;
    t.y = 0.0f;
    t.z = o->x5C.z - pos.z;
    o->x68 = func_800005C8(fwd, t);
}
