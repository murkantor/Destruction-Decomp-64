#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f r[3]; } Mtx3f;
typedef struct {
    u8 pad[0x2C];
    Vec3f x2C;
    u8 pad38[0x50 - 0x38];
    Mtx3f x50;
    u8 pad74[0xA4 - 0x74];
    Vec3f xA4;
    Vec3f xB0;
    u8 padBC[0xC8 - 0xBC];
    Mtx3f xC8;
    u8 padEC[0x130 - 0xEC];
    f32 x130;
    f32 x134;
    f32 x138;
} Phys;
typedef struct { u8 pad[0x10]; s8 x10; } Ctrl;
typedef struct {
    u8 pad[0xD];
    u8 xD;
    u8 padE[0x30 - 0xE];
    s32 x30;
    u8 pad34[0x3C - 0x34];
    Ctrl *x3C;
    u8 pad40[0x1EC - 0x40];
    Phys phys;
} Car;
typedef struct { u8 pad[4]; s32 x4; u8 pad8[0x28 - 8]; s32 x28; f32 x2C; } ItemDef;
typedef struct {
    s16 x0;
    u8 pad2[2];
    Vec3f x4;
    u8 pad10[0x20 - 0x10];
    s32 x20;
    u8 pad24[0x34 - 0x24];
    u8 x34[0xC];
    Vec3f x40;
    Vec3f x4C;
    s32 x58;
    f32 x5C;
    s32 x60;
    s32 x64;
    s32 x68;
    ItemDef *x6C;
} Pickup;
typedef struct {
    s16 x0;
    u8 pad2[6];
    f32 x8;
    u8 padC[4];
    f32 x10;
    f32 x14;
    u8 pad18[4];
    f32 x1C;
    s32 x20;
} Item;
typedef struct Node { Item *x0; struct Node *x4; } Node;
typedef struct { u8 pad[0x2C]; Node **x2C; } TrackC;
typedef struct { u8 pad[0xC]; TrackC *xC; } Track70;
typedef struct { u8 pad[0x70]; Track70 *x70; } Track;
typedef struct { u8 pad[0x10FC]; s16 x10FC; u8 pad10FE[2]; Track *x1100; } Game;
typedef struct { Car *x0; s32 x4; s32 x8; s32 xC; f32 x10; s32 x14; } Ev;

extern Game *gGame;
extern f64 D_800B92E0; /* 1.0 */
extern void *D_8011EEF8;
extern s32 func_80011CCC(Vec3f);
extern Vec3f Vec3f_TransformTransposed(Mtx3f *m, Vec3f *t, Vec3f *v);
extern u8 Seg2D_Intersects(f32, f32, f32, f32, f32, f32, f32, f32);
extern void func_8002B7A0(Car *, Item *, s32);
extern u8 func_800330A8(void *, void *, void *, s32, void *);
extern void func_80006E8C(void *, Ev *);
extern void Pad_Rumble(s32 channel, s32 frames, f32 strength);
extern void Vec3f_Scale(Vec3f *, Vec3f *, f32);
extern void Vec3f_Length(f32 *, Vec3f *);
extern void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Normalize(Vec3f *, Vec3f *);
extern void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
extern void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
extern s32 rand(void);

void Car_CheckTriggersAndPickups(Car *car) {
    Vec3f pts[10];
    Vec3f c[4];
    Item *item;
    f32 A, B, C, D;
    u8 done;
    Node *node;
    Phys *ph;
    TrackC *tc;
    Pickup *pk;
    Vec3f mid;
    s32 flags;
    s32 k;
    s32 start;
    s32 n;

    tc = gGame->x1100->x70->xC;
    pts[0].x = car->phys.x2C.x;
    pts[0].y = car->phys.x2C.y;
    pts[0].z = car->phys.x2C.z;
    pts[5].x = car->phys.xA4.x;
    pts[5].y = car->phys.xA4.y;
    pts[5].z = car->phys.xA4.z;
    mid.x = (pts[0].x + pts[5].x) * 0.5f;
    mid.y = (pts[0].y + pts[5].y) * 0.5f;
    mid.z = (pts[0].z + pts[5].z) * 0.5f;
    node = tc->x2C[func_80011CCC(mid)];
    ph = &car->phys;
    done = 0;
    for (; node != NULL; node = node->x4) {
        if (node->x0->x0 == 0x1A) {
            item = node->x0;
            flags = item->x20;
            if (!(flags & 2) && (!(flags & 1) || car->x30 != 0)) {
                continue;
            }
            A = item->x8;
            B = item->x10;
            C = item->x14;
            D = item->x1C;
            if (flags & 8) {
                if (!done) {
                    c[0].x = ph->x130;
                    c[0].y = 0.0f;
                    c[0].z = -ph->x138;
                    c[1].x = ph->x130;
                    c[1].y = 0.0f;
                    c[1].z = ph->x138;
                    c[2].x = -ph->x130;
                    c[2].y = 0.0f;
                    c[2].z = ph->x138;
                    c[3].x = -ph->x130;
                    c[3].y = 0.0f;
                    c[3].z = -ph->x138;
                    pts[1] = Vec3f_TransformTransposed(&car->phys.x50, &car->phys.x2C, &c[0]);
                    pts[2] = Vec3f_TransformTransposed(&car->phys.x50, &car->phys.x2C, &c[1]);
                    pts[3] = Vec3f_TransformTransposed(&car->phys.x50, &car->phys.x2C, &c[2]);
                    pts[4] = Vec3f_TransformTransposed(&car->phys.x50, &car->phys.x2C, &c[3]);
                    pts[6] = Vec3f_TransformTransposed(&car->phys.xC8, &car->phys.xA4, &c[0]);
                    pts[7] = Vec3f_TransformTransposed(&car->phys.xC8, &car->phys.xA4, &c[1]);
                    pts[8] = Vec3f_TransformTransposed(&car->phys.xC8, &car->phys.xA4, &c[2]);
                    pts[9] = Vec3f_TransformTransposed(&car->phys.xC8, &car->phys.xA4, &c[3]);
                    done = 1;
                }
                start = 1;
                n = 5;
            } else {
                start = 0;
                n = 1;
            }
            for (k = start; k < n; k++) {
                f32 px = pts[k].x;
                f32 pz = pts[k].z;
                f32 qx = pts[k + 5].x;
                f32 qz = pts[k + 5].z;
                if (Seg2D_Intersects(px, pz, qx, qz, A, B, C, D)) {
                    func_8002B7A0(car, item, (qz - pz) * (A - C) - (qx - px) * (B - D) > 0.0);
                }
            }
        } else if (node->x0->x0 == 0x1F) {
            Ev ev;
            Vec3f tmp;
            f32 len;
            pk = (Pickup *)node->x0;
            if (pk->x64 != 0 || pk->x60 == 0) {
                continue;
            }
            if (!func_800330A8(&ph->x130, &ph->xC8, &ph->xA4, pk->x6C->x4, pk->x34)) {
                continue;
            }
            pk->x58 = 0;
            pk->x64 = 1;
            ev.x0 = car;
            ev.x8 = 0;
            ev.x4 = 0;
            ev.x10 = pk->x6C->x2C;
            ev.x14 = 1;
            ev.xC = pk->x6C->x28;
            func_80006E8C(D_8011EEF8, &ev);
            if (car->x30 == 0 && car->xD == 0 && gGame->x10FC == 0x1C) {
                Pad_Rumble(car->x3C->x10, 5, 0.75f);
            }
            switch (pk->x68) {
            case 0: {
                f32 f;
                Vec3f_Scale(&pk->x40, &ph->xB0, 0.3f);
                pk->x40.y += 5.0f;
                Vec3f_Length(&len, &ph->xB0);
                f = len * 0.15;
                if (f > 10.0) {
                    f = 10.0f;
                }
                pk->x40.y += f;
                Vec3f_Sub(&tmp, &pk->x4, &ph->xA4);
                Vec3f_Normalize(&tmp, &tmp);
                Vec3f_Scale(&tmp, &tmp, (len + 10.0f) * 0.2);
                Vec3f_Add(&pk->x40, &pk->x40, &tmp);
                pk->x4C.x = rand() * (1.0f / 32767.0f) - 0.5;
                pk->x4C.y = 0.0f;
                pk->x4C.z = rand() * (1.0f / 32767.0f) - 0.5;
                Vec3f_Normalize(&pk->x4C, &pk->x4C);
                Vec3f_Scale(&pk->x4C, &pk->x4C, (len + 25.0) * ((rand() * (1.0f / 32767.0f) + 1.0) * 0.1));
                Vec3f_Length(&len, &pk->x40);
                tmp.x = rand() * (1.0f / 32767.0f) - 0.5;
                tmp.y = rand() * (1.0f / 32767.0f) - 0.5;
                tmp.z = rand() * (1.0f / 32767.0f) - 0.5;
                Vec3f_Scale(&tmp, &tmp, len * 0.5);
                Vec3f_Add(&pk->x40, &pk->x40, &tmp);
            } break;
            case 1: {
                Vec3f d;
                Vec3f ax;
                Vec3f up = { 0.0f, 1.0f, 0.0f };
                f32 len2;
                Vec3f_Sub(&d, &pk->x4, &ph->xA4);
                Vec3f_Normalize(&d, &d);
                Vec3f_Scale(&d, &d, 5.0f);
                Vec3f_Add(&d, &d, &ph->xB0);
                Vec3f_Length(&len2, &d);
                d.y = 0.0f;
                Vec3f_Normalize(&d, &d);
                Vec3f_Cross(&ax, &up, &d);
                Vec3f_Scale(&ax, &ax, 1.57075f);
                if (len2 < 15.0) {
                    len2 = 15.0f;
                } else if (len2 > 50.0) {
                    len2 = 50.0f;
                }
                Vec3f_Scale(&pk->x4C, &ax, len2 * 0.35);
                pk->x5C = D_800B92E0 / (len2 * 0.35);
            } break;
            }
        }
    }
}
