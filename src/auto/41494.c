#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern s32 D_80097350;
extern s32 D_800958A4;
extern s32 D_800958AC[];
extern s32 D_800967B0[][3];
extern s32 D_800967E0[][3];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);

void PakMenu_ShowSlotOptions(void) {
    u32 i;
    Ui_ResetScreen(0x112B6, 0x112B7);
    Ui_AddElement(gUi[0], 2, 0x3A);
    Ui_AddElement(gUi[0], 2, 0x87);
    for (i = 0; i < 4; i++) {
        if (i == D_800958AC[0]) {
            Ui_AddElement(*gUi, 2, D_800967B0[i][1]);
        } else if (i == 3 && D_800958AC[-2] >= 3) {
            Ui_AddElement(*gUi, 2, D_800967B0[3][2]);
        } else if (i == 2 && D_800958A4 == 3) {
            Ui_AddElement(*gUi, 2, D_800967B0[2][2]);
        } else {
            Ui_AddElement(*gUi, 2, D_800967B0[i][0]);
        }
    }
    Ui_AddElement(gUi[0], 2, D_800967E0[D_800958AC[-2] - 2][1]);
    if (D_800958AC[-2] == 2) {
        D_80097350 = 4;
    } else {
        D_80097350 = 3;
    }
}
