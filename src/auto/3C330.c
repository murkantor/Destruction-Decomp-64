#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern char gSaveSlotText[];
extern char D_800B9D78[];
extern char D_800B9D98[];
extern char D_800B9DB4[];
extern char D_800B9DC4[];
extern char D_800B9DDC[];
void Ui_AddElement(void *, s32, s32);
int sprintf(char *, const char *, ...);

void PakMenu_InsertPaks(void) {
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x132);
    sprintf(gSaveSlotText, D_800B9D78);
    sprintf(gSaveSlotText + 0x32, D_800B9D98);
    sprintf(gSaveSlotText + 0x64, D_800B9DB4);
    sprintf(gSaveSlotText + 0x96, D_800B9DC4);
    sprintf(gSaveSlotText + 0xC8, D_800B9DDC);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
}
