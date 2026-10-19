#include "common.h"
#include "m2c.h"

typedef struct { s32 w[18]; } UiDesc;
extern void *gUi[];
extern void Ui_AddSpriteElement(void *, UiDesc *);
extern void Ui_AddElement(void *, s32, s32);

void Ui_AddModeLabels(s32 mode) {
    s32 a1 = -1;
    s32 a2 = -1;
    s32 b1 = -1;
    s32 b2 = -1;

    UiDesc a = { { 0x03F20000, 0xC, 2, 0, 0, 0x11366, 0x11367, 0x434E0000, 0x42820000, 0, 0, 0x3F800000, 0x3F800000, 0, 0, 0, -1, 0 } };
    UiDesc b = { { 0x03F20000, 0xD, 2, 0, 0, 0x113DA, 0x113DB, 0x41200000, 0x41A00000, 0, 0, 0x3F800000, 0x3F800000, 0, 0, 0, 5, 0 } };
    switch (mode) {
    case 2:
        a1 = 0x11366;
        a2 = 0x11367;
        b1 = 0x113DC;
        b2 = 0x113DD;
        break;
    case 3:
        a1 = 0x113C4;
        a2 = 0x113C5;
        b1 = 0x113DE;
        b2 = 0x113DF;
        break;
    case 0:
        a1 = 0x11366;
        a2 = 0x11367;
        b1 = 0x113DA;
        b2 = 0x113DB;
        break;
    case 1:
        a1 = 0x11364;
        a2 = 0x11365;
        b1 = 0x113D8;
        b2 = 0x113D9;
        break;
    }
    a.w[5] = a1;
    a.w[6] = a2;
    Ui_AddSpriteElement(gUi[0], &a);
    Ui_AddElement(gUi[0], 2, 12);
    b.w[5] = b1;
    b.w[6] = b2;
    Ui_AddSpriteElement(gUi[0], &b);
    Ui_AddElement(gUi[0], 2, 13);
}
