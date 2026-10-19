#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 data[0x58]; } Rec;
typedef struct { s32 x0; s32 x4; s32 x8; u8 xC; u8 pad[3]; } Track;
typedef struct { u8 pad[0x198]; s32 x198; u8 pad2[0xBA8 - 0x19C]; f32 xBA8; } Car;
typedef struct { u8 pad[0x74]; Car *x74[12]; } Race;
typedef struct { u8 pad[0x1058]; u64 x1058; u8 pad2[0x1100 - 0x1060]; Race *x1100; } Game;
typedef struct {
    u8 pad[2];
    s16 x2;
    u8 pad4[0xC];
    u16 x10;
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    u8 pad30[4];
    f32 x34;
} Ent;
extern Game *gGame;
extern Track D_801311D0[];
extern s32 D_801320C8[];
extern f32 D_8007EEC0, D_8007EEC4, D_8007EEC8;
extern s32 CarEffect_Init(s32, s32, Rec *);
extern s8 Pool_PushCopy(void *, Rec *);
extern Ent *func_8005CC00(void *, s16);
extern f32 func_80063718(u64 *);
extern Vec3f func_8001FBA8(Car *);
extern Vec3f func_8001FB88(Car *);
extern void List_RemoveById(void *, s16);

void EngineSfx_Crossfade(void *list, s32 i, s32 snd) {
    Track *t = &D_801311D0[i];
    Car *car = gGame->x1100->x74[i];
    Rec rec;
    s32 id;
    Ent *e;
    f32 d;
    f32 v;

    if (t->xC) {
        id = (s16)CarEffect_Init(i, snd, &rec);
        if (!Pool_PushCopy(list, &rec)) {
            return;
        }
        t->x4 = t->x0;
        t->x0 = id;
        t->xC = 0;
    }
    e = func_8005CC00(list, t->x0);
    if (e == NULL) {
        return;
    }
    if (e->x14 < D_801320C8[e->x10]) {
        e->x14 += (s32)(D_801320C8[e->x10] / D_8007EEC0 * func_80063718(&gGame->x1058));
        if (D_801320C8[e->x10] < e->x14) {
            e->x14 = D_801320C8[e->x10];
        }
    }
    d = car->xBA8 - 1000.0;
    if (t->x8) {
        v = d / (f32)(car->x198 - 1000.0) * (D_8007EEC4 - D_8007EEC8) + D_8007EEC8;
    } else {
        v = 0.0f;
    }
    e->x34 = v;
    e->x18 = func_8001FBA8(car);
    e->x24 = func_8001FB88(car);
    if (t->x4 == 0) {
        return;
    }
    e = func_8005CC00(list, t->x4);
    if (e == NULL) {
        return;
    }
    if (e->x14 > 0) {
        e->x14 -= (s32)(D_801320C8[e->x10] / D_8007EEC0 * func_80063718(&gGame->x1058));
        if (e->x14 < 0) {
            e->x14 = 0;
        }
        e->x18 = func_8001FBA8(car);
        e->x24 = func_8001FB88(car);
    } else {
        List_RemoveById(list, e->x2);
        t->x4 = 0;
    }
}
