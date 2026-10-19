#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    u8 pad4[0x64];
} Slot3E324;

extern void *gUi[];
extern char D_80123C56[];
extern char D_800BA308[];
extern char D_800BA318[];
extern char D_800BA324[];
extern char D_800BA330[];
extern char D_800BA340[];
extern s32 D_80097350;
extern Slot3E324 D_80095908[];

void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);

void func_8003E324(void) {
    s8 i;

    Ui_ResetScreen(0x11427, 0x11428);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x130);
    sprintf(D_80123C56, D_800BA308);
    sprintf(D_80123C56 + 0x32, D_800BA318);
    sprintf(D_80123C56 - 0x96, D_800BA324);
    sprintf(D_80123C56 - 0x64, D_800BA330);
    sprintf(D_80123C56 - 0x32, D_800BA340);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    D_80097350 = 3;
    for (i = 0; i < 4; i++) {
        D_80095908[i].x0 = 0;
    }
}
