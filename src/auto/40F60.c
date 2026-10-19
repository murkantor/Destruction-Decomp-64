#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } Entry;
typedef struct { s32 x0; s32 x4; u8 pad[0x60]; } Slot;
extern void *gUi[];
extern s32 D_80097350;
extern s32 D_800958A4;
extern Entry D_800967E0[];
extern Entry D_800967B0[];
extern Slot D_80095904[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);

void func_80040360(void) {
    s32 i;

    Ui_ResetScreen(0x112B6, 0x112B7);
    Ui_AddElement(gUi[0], 2, 0x3A);
    Ui_AddElement(gUi[0], 2, 0x87);
    D_80097350 = 3;
    for (i = 0; i < D_80097350; i++) {
        if (i == 0) {
            Ui_AddElement(gUi[0], 2, D_800967E0[0].x4);
        } else {
            Ui_AddElement(gUi[0], 2, D_800967E0[i].x0);
        }
    }
    for (i = 0; i < 4; i++) {
        Ui_AddElement(gUi[0], 2, D_800967B0[i].x0);
    }
    D_800958A4 = 4;
    for (i = 0; i < 4; i++) {
        D_80095904[i].x0 = 2;
        D_80095904[i].x4 = 0;
    }
}
