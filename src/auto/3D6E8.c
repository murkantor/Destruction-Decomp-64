#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern u8 D_8009734C;
extern u8 D_8009734D;
extern s32 D_80097350;
extern char D_80123CBA[];
extern char D_800B9EB4[], D_800B9ECC[], D_800B9EE8[], D_800B9EB0[], D_800B9EAC[], D_800B9F04[], D_800B9F28[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
int sprintf(char *, const char *, ...);

void PakMenu_NoSaveWarning(void) {
    Ui_ResetScreen(0x11580, 0x11581);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    Ui_AddElement(gUi[0], 0, 0x17F);
    sprintf(D_80123CBA, D_800B9EB4);
    sprintf(D_80123CBA - 0xFA, D_800B9ECC);
    sprintf(D_80123CBA - 0x96, D_800B9EE8);
    sprintf(D_80123CBA - 0x64, D_800B9EB0);
    sprintf(D_80123CBA - 0x32, D_800B9EAC);
    if (D_8009734C || D_8009734D) {
        Ui_AddElement(gUi[0], 2, 0x130);
        sprintf(D_80123CBA - 0xC8, D_800B9F04);
    } else {
        sprintf(D_80123CBA - 0xC8, D_800B9F28);
        Ui_SetElementPos(gUi[0], 0, 0x17B, 81.0f, 120.0f);
    }
    D_80097350 = 2;
}
