#include "common.h"
#include "m2c.h"

typedef struct { u32 a : 8; u32 b : 8; u32 c : 16; } PakFlags;
extern void *gUi[];
extern char gSaveSlotText[];
extern u8 D_8009734C;
extern s32 D_80097350;
extern char D_800B9E04[];
extern char D_800B9E20[];
extern char D_800B9E40[];
extern char D_800B9E54[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
int sprintf(char *, const char *, ...);

void PakMenu_NoPak(void) {
    Ui_ResetScreen(0x112C6, 0x112C7);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x130);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    sprintf(gSaveSlotText, D_800B9E04);
    sprintf(gSaveSlotText + 0x32, D_800B9E20);
    if (D_8009734C) {
        sprintf(gSaveSlotText + 0x64, D_800B9E40);
    } else {
        sprintf(gSaveSlotText + 0x64, D_800B9E54);
    }
    if (!((PakFlags *)&D_8009734C)->a && !((PakFlags *)&D_8009734C)->b) {
        ((PakFlags *)&D_8009734C)->a = 1;
    }
    D_80097350 = 0;
}
