#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad[0x64]; } Slot68;
extern char gSaveSlotText[][0x32];
extern char D_800B9FC0[], D_800B9FE8[], D_800BA014[], D_800B9EB0[], D_800B9EAC[];
extern void *gUi[];
extern s32 D_80097350;
extern Slot68 D_80095908[];
void func_8003EA7C(s32, s32);
s32 sprintf(char *, const char *, ...);
void Ui_AddElement(void *, s32, s32);

void PakMenu_ConfirmOverwrite(void) {
    s32 i;

    func_8003EA7C(0x115A0, 0x115A1);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x137);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    sprintf(gSaveSlotText[0], D_800B9FC0);
    sprintf(gSaveSlotText[1], D_800B9FE8);
    sprintf(gSaveSlotText[2], D_800BA014);
    sprintf(gSaveSlotText[3], D_800B9EB0);
    sprintf(gSaveSlotText[4], D_800B9EAC);
    D_80097350 = 2;
    for (i = 0x138; i >= 0; i -= 0x68) {
        *(s32 *)((u8 *)D_80095908 + i) = 0;
    }
}
