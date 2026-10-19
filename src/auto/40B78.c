#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } MenuItem;
extern void *gUi;
extern s32 D_80097350;
extern s32 D_800967E0[][3];
extern s32 D_800967B0[][3];
void Timer_Start(void *, s64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

s32 func_8003FF78(MenuItem *it) {
    s32 old = it->x10;
    s32 n;
    s32 (*tbl)[3] = D_800967E0;
    s32 next;

    Timer_Start(it, 150000000);
    n = D_80097350;
    if (n != 0) {
        next = (it->x10 + 1) % n;
        if (it->x10 != next) {
            Ui_RemoveElement(gUi, tbl[it->x10][1]);
            Ui_RemoveElement(gUi, tbl[next][0]);
            Ui_AddElement(gUi, 2, tbl[next][1]);
            Ui_AddElement(gUi, 2, tbl[it->x10][0]);
            it->x10 = next;
        }
    }
    if (old == it->x10) {
        return 0;
    }
    switch (it->x10) {
    case 0: {
        void **ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967B0[3][0]);
        Ui_RemoveElement(*ui, D_800967B0[3][2]);
        break;
    }
    case 1: {
        void **ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967B0[2][2]);
        Ui_RemoveElement(*ui, D_800967B0[2][0]);
        Ui_AddElement(*ui, 2, D_800967B0[3][2]);
        Ui_RemoveElement(*ui, D_800967B0[3][0]);
        break;
    }
    case 2: {
        void **ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967B0[2][0]);
        Ui_RemoveElement(*ui, D_800967B0[2][2]);
        break;
    }
    }
    return 1;
}
