#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { s32 x0; s32 x4; s32 x8; } MenuItem;
extern void *gUi[];
extern s32 D_80097350;
void Timer_Start(void *, s64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

void func_8003EEF0(Menu *m, MenuItem *items) {
    s32 prev;

    Timer_Start(m, 150000000);
    if (D_80097350 == 0) {
        return;
    }
    if (m->x10 == 0) {
        prev = D_80097350 - 1;
    } else {
        prev = m->x10 - 1;
    }
    if (m->x10 != prev) {
        Ui_RemoveElement(gUi[0], items[m->x10].x4);
        Ui_RemoveElement(gUi[0], items[prev].x0);
        Ui_AddElement(gUi[0], 2, items[prev].x4);
        Ui_AddElement(gUi[0], 2, items[m->x10].x0);
        m->x10 = prev;
    }
}
