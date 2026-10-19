#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern char gSaveSlotText[];
extern char D_800BA764[];
extern char D_800BA780[];
extern s32 D_80097350;
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
int sprintf(char *, const char *, ...);

void func_80043230(void) {
    Ui_ResetScreen(0x112FC, 0x112FD);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    sprintf(gSaveSlotText, D_800BA764);
    sprintf(gSaveSlotText + 0x32, D_800BA780);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    D_80097350 = 0;
}
