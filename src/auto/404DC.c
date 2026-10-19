#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } UnkObj;
void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
extern s32 D_80097A14;
extern u8 D_801253F9;
extern s32 D_80097350;
extern s32 D_80096780[][3];
extern void *gUi;

s32 func_8003F8DC(UnkObj *o) {
    s32 prev;
    s32 (*tbl)[3];
    if (D_80097A14 == 0 && D_801253F9 == 0) {
        return 0;
    }
    Timer_Start(o, 150000000);
    tbl = D_80096780;
    if (D_80097350 == 0) {
        return 1;
    }
    prev = (o->x10 == 0) ? D_80097350 - 1 : o->x10 - 1;
    if (o->x10 != prev) {
        Ui_RemoveElement(gUi, tbl[o->x10][1]);
        Ui_RemoveElement(gUi, tbl[prev][0]);
        Ui_AddElement(gUi, 2, tbl[prev][1]);
        Ui_AddElement(gUi, 2, tbl[o->x10][0]);
        o->x10 = prev;
    }
    return 1;
}
