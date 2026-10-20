#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec2f;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    u8 x10;
    s8 x11;
    s8 x12;
    s8 x13;
    u8 pad14[4];
    u64 x18;
    u8 x20;
    u8 pad21[7];
} CountState;
typedef struct {
    u64 time;
    u8 x8;
    u8 pad9[3];
    f32 xC;
    f32 x10;
    u8 x14;
    u8 pad15[3];
} CountFx;
typedef struct { u8 pad0[0x10]; s32 x10; } Score;
typedef struct { Score *x0; u8 pad4[0x64]; } ScoreSlot;
typedef struct {
    s32 x0;
    u8 pad4[0x14];
    u8 x18[0x6C];
    s32 x84;
    s32 x88;
    s32 x8C;
} UiElem;
typedef struct { u8 pad0[0x10A8]; u8 x10A8[0x54]; s16 x10FC; } Game;
typedef struct { s32 x0; } Arg;
extern Game *gGame;
extern void *gUi[];
extern CountState D_80125FC8[];
extern ScoreSlot D_800958F0[];
extern CountFx D_8007EEE8[];
extern s8 D_800D31CC;
UiElem *Ui_FindElement(void *, s32, s32);
void Ui_GetElementPos(Vec2f *, void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void func_8005D8B8(void *, s32, s32);
void func_80061598(void *, s32, s32);
s32 Hud_HealthBarStep(CountState *);
u64 Time_Get(void *);

s32 Hud_UpdateHealthBar(Arg *arg0, s8 idx) {
    Vec2f pos;
    s32 a[2] = { 0x36, 0x37 };
    s32 b[2] = { 0x34, 0x35 };
    s32 unused[2];
    s32 ret = 0;
    UiElem *el;
    CountState *st;
    Score *t;
    s32 d;

    st = &D_80125FC8[idx];
    el = Ui_FindElement(gUi[0], 2, arg0->x0);
    t = D_800958F0[idx].x0;
    if (!st->x20) {
        if (gGame->x10FC == 0x1F) {
            el->x8C = st->x12;
            func_80061598(el->x18, 0, st->x0 - st->x12);
            st->x20 = 1;
            return 0;
        }
        if (D_800D31CC < 3) {
            st->x10 = 0;
            D_8007EEE8[idx].time = 0;
            D_8007EEE8[idx].x14 = 0;
            D_8007EEE8[idx].x8 = 0;
            Ui_GetElementPos(&pos, gUi[0], 2, b[idx]);
            D_8007EEE8[idx].xC = pos.x;
            D_8007EEE8[idx].x10 = pos.y;
        } else {
            st->x10 = 0;
        }
        st->x11 = el->x8C;
        st->x12 = el->x8C;
        st->x8 = t->x10;
        st->xC = t->x10;
        st->x13 = el->x8C * 0.316;
        st->x4 = el->x88;
        st->x0 = el->x8C;
        st->x20 = 1;
    }
    if (t->x10 < 0) {
        t->x10 = 0;
    }
    if (st->xC == t->x10) {
        if (st->x12 < st->x11) {
            goto calc;
        }
        goto skip;
    }
    st->x18 = Time_Get(gGame->x10A8);
    st->xC = t->x10;
calc:
    d = Hud_HealthBarStep(st);
    el->x84 += d;
    el->x8C -= d;
    if (el->x8C == 0) {
        Ui_RemoveElement(gUi[0], el->x0);
        ret = 1;
    } else {
        func_80061598(el->x18, 0, st->x0 - st->x11);
    }
skip:
    if (D_800D31CC < 3 && !(st->x13 < st->x11)) {
        Ui_RemoveElement(gUi[0], b[idx]);
        ret = 1;
        func_8005D8B8(gUi[0], 2, a[idx]);
        Ui_GetElementPos(&pos, gUi[0], 2, a[idx]);
        D_8007EEE8[idx].time = Time_Get(gGame->x10A8) + 250000000.0;
        D_8007EEE8[idx].x8 = 1;
        D_8007EEE8[idx].xC = pos.x;
        D_8007EEE8[idx].x10 = pos.y;
        D_8007EEE8[idx].x14 = 1;
        st->x13 = -1;
    }
    return ret;
}
