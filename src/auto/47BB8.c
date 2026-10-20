#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad[0x64]; } Slot;

extern void *gUi[];
extern char gSaveSlotText[];
extern char D_800BACC8[];
extern char D_800BACE8[];
extern char D_800BACEC[];
extern Slot D_80095908[];
extern s32 D_80097350;

void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void Ui_SetElementColor(s32, s32);
int sprintf(char *, const char *, ...);

void Menu_ShowExitConfirm(void) {
    s8 i;

    Ui_RemoveElement(gUi[0], 0x1A7);
    Ui_RemoveElement(gUi[0], 0x1A6);
    Ui_RemoveElement(gUi[0], 0x38);
    Ui_RemoveElement(gUi[0], 0x1B0);
    Ui_RemoveElement(gUi[0], 0x1B1);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    sprintf(gSaveSlotText, D_800BACC8);
    sprintf(gSaveSlotText + 0x64, D_800BACE8);
    sprintf(gSaveSlotText + 0x96, D_800BACEC);
    Ui_SetElementPos(gUi[0], 0, 0x17A, 57.0f, 120.0f);
    Ui_SetElementPos(gUi[0], 0, 0x17C, 151.0f, 150.0f);
    Ui_SetElementPos(gUi[0], 0, 0x17D, 148.0f, 166.0f);
    Ui_SetElementColor(0x17A, 0x111F8);
    Ui_SetElementColor(0x17C, 0x111F4);
    Ui_SetElementColor(0x17D, 0x111F8);
    for (i = 0; i < 4; i++) {
        D_80095908[i].x0 = 0;
    }
    D_80097350 = 2;
}
