#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { u8 pad0[0x30]; Seg *segs; } World;
typedef struct { u8 pad0[0x10FC]; s16 x10FC; u8 pad10FE[2]; World *x1100; } Game;
typedef struct { u8 pad0[0x10]; s8 x10; } PadInfo;
typedef struct {
    u8 pad0[0x28];
    s32 seg;
    u8 pad2C[4];
    s32 x30;
    s32 x34;
    u8 pad38[4];
    PadInfo *x3C;
    u8 x40;
    u8 pad41[0x17];
    s32 dir;
    u8 pad5C[0x290 - 0x5C];
    Vec3f pos;
} Car;

extern Game *gGame[];
extern void *gUi;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B4;
extern s32 D_800D1C20[];
extern Vec3f D_800D1B08[];
extern f64 D_800B8AB0;
extern f64 D_800B8AB8;
extern f64 D_800B8AC0;
extern f64 D_800B8AC8;
extern u8 D_80091B14[];
extern s32 D_80091B18[];

f32 Vec3f_DistXZ(Vec3f, Vec3f);
u8 Seg2D_Intersects(f32, f32, f32, f32, f32, f32, f32, f32);
s32 Racer_WrapNode(Car *, s32);
void Race_AddNodeDistance(Car *, s32);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);
void Announcer_QueueRandom(s32 *, s32, s32, s32, s32);

#define CHECK(n, K)                                                                       \
    (d = Vec3f_DistXZ(w->segs[n].a, w->segs[n].b),                                        \
     p0.x = w->segs[n].a.x + (w->segs[n].b.x - w->segs[n].a.x) / d * K,                   \
     p0.z = w->segs[n].a.z + (w->segs[n].b.z - w->segs[n].a.z) / d * K,                   \
     p1.x = w->segs[n].b.x + (w->segs[n].a.x - w->segs[n].b.x) / d * K,                   \
     p1.z = w->segs[n].b.z + (w->segs[n].a.z - w->segs[n].b.z) / d * K,                   \
     Seg2D_Intersects(D_800D1B08[slot].x, D_800D1B08[slot].z, pos.x, pos.z, p0.x, p0.z, p1.x, p1.z))

#define CROSS()                                                                                  \
    ca = D_800D1B08[slot].z - pos.z;                                                             \
    cb = p0.x - p1.x;                                                                            \
    cc = D_800D1B08[slot].x - pos.x;                                                             \
    ce = p0.z - p1.z;                                                                            \
    cross = (ca * cb - cc * ce > 0.0);                                                           \
    if (D_800958B4 == 7)                                                                         \
        cross ^= 1;                                                                              \
    if (D_800958B4 == 8 && seg >= 0x51 && seg < 0x145)                                 \
        cross ^= 1;

void Car_UpdateTrackSegment(Car *car)
{
    s32 id;
    s32 dir;
    World *w;
    s32 seg;
    u8 flag;
    s32 slot;
    s32 i;
    s32 n;
    Vec3f pos;
    Vec3f p0;
    Vec3f p1;
    f32 d;
    s32 cross;
    f32 ca, cb, cc, ce;
    s32 ids[6];
    u8 fwd1;
    u8 fwd2;
    s32 *pi;

    dir = car->dir;
    flag = 0;
    w = gGame[0]->x1100;
    seg = car->seg;
    id = car->x34;

    if (D_800958A8 == 3 && D_800958AC == 2) {
        i = id;
    } else {
        for (i = 0; i < 12; i++) {
            if (D_800D1C20[i] == id)
                break;
        }
    }
    slot = i;
    PORT_CONVERT(&car->pos, 3, 4); /* copied into a local */
    pos = car->pos;
    if (D_800D1B08[slot].x == pos.x && D_800D1B08[slot].z == pos.z)
        return;

    if (CHECK(seg, D_800B8AB0)) {
        CROSS()
        fwd1 = dir == 1;
        if ((cross & fwd1) || (!(u8)cross & (dir == -1))) {
            n = Racer_WrapNode(car, car->seg + car->dir);
            if (car->x30 == 0) {
                if (gGame[0]->x10FC != 0x19) {
                    if (D_80091B14[car->x3C->x10]) {
                        D_80091B14[car->x3C->x10] = 0;
                        Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
                    }
                    D_80091B18[car->x3C->x10] = 0;
                }
                if (car->x40)
                    car->x40 = 0;
                if (car->x30 == 0)
                    D_80091B18[car->x3C->x10] = 0;
            }
            car->seg = n;
            flag = 1;
        } else {
            Race_AddNodeDistance(car, seg);
            flag = 1;
        }
    }

    if (car->x30 == 0 && !flag) {
        n = Racer_WrapNode(car, seg - dir);
        if (CHECK(n, D_800B8AB8)) {
            CROSS()
            fwd2 = dir == 1;
            if ((cross & fwd2) || (!(u8)cross & (dir == -1))) {
                if (car->x30 == 0) {
                    if (gGame[0]->x10FC != 0x19) {
                        if (D_80091B14[car->x3C->x10]) {
                            D_80091B14[car->x3C->x10] = 0;
                            Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
                        }
                        D_80091B18[car->x3C->x10] = 0;
                    }
                    if (car->x40)
                        car->x40 = 0;
                    if (car->x30 == 0)
                        D_80091B18[car->x3C->x10] = 0;
                }
                car->seg = n;
            } else {
                Race_AddNodeDistance(car, n);
            }
            flag = 1;
        }
        if (!flag && !car->x40) {
            n = Racer_WrapNode(car, seg + dir * 3);
            if (CHECK(n, D_800B8AC0)) {
                n = Racer_WrapNode(car, n + dir);
                car->seg = n;
                if (gGame[0]->x10FC != 0x19) {
                    if (D_80091B14[car->x3C->x10]) {
                        D_80091B14[car->x3C->x10] = 0;
                        Ui_RemoveElement(gUi, car->x3C->x10 + 0xE2);
                    }
                    D_80091B18[car->x3C->x10] = 0;
                }
                flag = 1;
            }
        }
        if (!flag && !car->x40) {
            n = Racer_WrapNode(car, seg - dir * 3);
            if (CHECK(n, D_800B8AC8)) {
                car->seg = n;
                pi = ids;
                pi[0] = 0x86;
                pi[1] = 0x87;
                if (gGame[0]->x10FC != 0x19) {
                    if (!D_80091B14[car->x3C->x10]) {
                        D_80091B14[car->x3C->x10] = 1;
                        Ui_AddElement(gUi, 2, car->x3C->x10 + 0xE2);
                        if (gGame[0]->x10FC == 0x1C) {
                            if (D_800958A8 == 2)
                                Msg_Queue(0x87, 0, 1, 0);
                            else
                                Announcer_QueueRandom(pi, 2, 0, 1, 0);
                        }
                    }
                }
            }
        }
    }
    D_800D1B08[slot].x = pos.x;
    D_800D1B08[slot].z = pos.z;
}
