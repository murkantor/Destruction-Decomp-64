#include "common.h"
#include "m2c.h"

extern char gSaveSlotText[][0x32];
extern char D_800BA238[], D_800BA250[], D_800BA270[], D_800BA2A8[], D_800BA2C0[], D_800BA2DC[], D_800BA2F8[];
extern void *gUi[];
extern s32 D_80097350;
void Ui_ResetScreen(s32, s32);
s32 sprintf(char *, const char *, ...);
void Ui_AddElement(void *, s32, s32);

void func_8003DF0C(void) {
    Ui_ResetScreen(0x112CA, 0x112CB);
    sprintf(gSaveSlotText[0], D_800BA238);
    sprintf(gSaveSlotText[1], D_800BA250);
    sprintf(gSaveSlotText[2], D_800BA270);
    sprintf(gSaveSlotText[3], D_800BA2A8);
    sprintf(gSaveSlotText[4], D_800BA2C0);
    sprintf(gSaveSlotText[5], D_800BA2DC);
    sprintf(gSaveSlotText[6], D_800BA2F8);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    Ui_AddElement(gUi[0], 0, 0x17F);
    Ui_AddElement(gUi[0], 0, 0x180);
    D_80097350 = 0;
}
