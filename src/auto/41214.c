#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } UiPair;
typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern UiPair D_800967B0[];
extern s32 D_80097350;
extern s32 D_800958A4;
extern void *gUi[];
void Timer_Start(void *, s64);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);

s32 func_80040614(Menu *m) {
    u8 done = 0;
    s32 idx;

    Timer_Start(m, 300000000);
    idx = m->x10;
    do {
        idx = (idx + 1) % D_80097350;
        switch (D_800958A4) {
            case 2:
                done = 1;
                break;
            case 3:
                if (idx != 2 && idx != 3) done = 1;
                break;
            case 4:
                if (idx != 3) done = 1;
                break;
        }
    } while (!done);
    Ui_RemoveElement(gUi[0], D_800967B0[m->x10].x4);
    Ui_RemoveElement(gUi[0], D_800967B0[idx].x0);
    Ui_AddElement(gUi[0], 2, D_800967B0[idx].x4);
    Ui_AddElement(gUi[0], 2, D_800967B0[m->x10].x0);
    m->x10 = idx;
    return 1;
}
