#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern char gSaveSlotText[];
extern char D_800BA79C[];
extern s32 D_80097350;
void func_8003EA7C(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

void func_800432E0(void) {
    func_8003EA7C(0x112FC, 0x112FD);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    sprintf(gSaveSlotText, D_800BA79C);
    Ui_SetElementPos(gUi[0], 0, 0x17A, 114.0f, 104.0f);
    Ui_AddElement(gUi[0], 0, 0x17A);
    D_80097350 = 0;
}
