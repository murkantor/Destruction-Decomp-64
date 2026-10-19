#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } UnkObj;
void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
extern s32 D_80097350;
extern s32 D_80096750[][3];
extern void *gUi;
extern s32 D_800958A8;

s32 func_8003F100(UnkObj *o) {
    s32 prev;
    s32 (*tbl)[3];
    Timer_Start(o, 150000000);
    tbl = D_80096750;
    if (D_80097350 != 0) {
        prev = (o->x10 == 0) ? D_80097350 - 1 : o->x10 - 1;
        if (o->x10 != prev) {
            Ui_RemoveElement(gUi, tbl[o->x10][1]);
            Ui_RemoveElement(gUi, tbl[prev][0]);
            Ui_AddElement(gUi, 2, tbl[prev][1]);
            Ui_AddElement(gUi, 2, tbl[o->x10][0]);
            o->x10 = prev;
        }
    }
    D_800958A8 = o->x10;
    return 1;
}
