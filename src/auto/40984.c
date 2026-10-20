#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } UiPair;
typedef struct { u8 timer[0x10]; s32 sel; } Menu;
extern void *gUi;
extern s32 D_80097350;
extern UiPair D_800967E0[];
extern s32 D_800967C8, D_800967D0, D_800967D4, D_800967DC;
void Timer_Start(void *, u64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

s32 PlayerMenu_Prev(Menu *m) {
    s32 old = m->sel;
    s32 next;
    void **ui;
    UiPair *pairs = D_800967E0;

    Timer_Start(m, 150000000);
    if (D_80097350 != 0) {
        if (m->sel != 0) {
            next = m->sel - 1;
        } else {
            next = D_80097350 - 1;
        }
        if (m->sel != next) {
            Ui_RemoveElement(gUi, pairs[m->sel].x4);
            Ui_RemoveElement(gUi, pairs[next].x0);
            Ui_AddElement(gUi, 2, pairs[next].x4);
            Ui_AddElement(gUi, 2, pairs[m->sel].x0);
            m->sel = next;
        }
    }
    if (old == m->sel) {
        return 0;
    }
    switch (m->sel) {
    case 0:
        ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967D4);
        Ui_RemoveElement(*ui, D_800967DC);
        Ui_AddElement(*ui, 2, D_800967C8);
        Ui_RemoveElement(*ui, D_800967D0);
        break;
    case 1:
        ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967D0);
        Ui_RemoveElement(*ui, D_800967C8);
        break;
    case 2:
        ui = &gUi;
        Ui_AddElement(*ui, 2, D_800967DC);
        Ui_RemoveElement(*ui, D_800967D4);
        break;
    }
    return 1;
}
