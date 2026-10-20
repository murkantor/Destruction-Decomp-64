#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } Pair12;
typedef struct { u8 pad[0x10]; s32 x10; } Cycler;
extern s32 D_80097A14;
extern u8 D_801253F9;
extern s32 D_80097350;
extern Pair12 D_80096780[];
extern void *gUi;
void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);

s32 DiffMenu_Next(Cycler *c) {
    s32 n;
    s32 next;
    Pair12 *tbl;

    if (D_80097A14 == 0 && D_801253F9 == 0) {
        return 0;
    }
    Timer_Start(c, 150000000);
    n = D_80097350;
    tbl = D_80096780;
    if (n == 0) {
        return 1;
    }
    next = (c->x10 + 1) % n;
    if (c->x10 != next) {
        Ui_RemoveElement(gUi, tbl[c->x10].x4);
        Ui_RemoveElement(gUi, tbl[next].x0);
        Ui_AddElement(gUi, 2, tbl[next].x4);
        Ui_AddElement(gUi, 2, tbl[c->x10].x0);
        c->x10 = next;
    }
    return 1;
}
