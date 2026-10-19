#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 pad[0x60]; } Slot;
extern void *gUi[];
extern s32 D_80097350;
extern Slot D_80095904[];
extern char gSaveSlotText[][0x32];
extern char D_800B9F40[];
extern char D_800B9F4C[];
extern char D_800B9F74[];
extern char D_800B9F8C[];
extern char D_800B9FA4[];
void func_8003EA7C(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);

void PakMenu_NoteExists(void) {
    s32 i;

    func_8003EA7C(0x1159E, 0x1159F);
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x130);
    Ui_AddElement(gUi[0], 2, 0x137);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    sprintf(gSaveSlotText[0], D_800B9F40);
    sprintf(gSaveSlotText[1], D_800B9F4C);
    sprintf(gSaveSlotText[2], D_800B9F74);
    sprintf(gSaveSlotText[3], D_800B9F8C);
    sprintf(gSaveSlotText[4], D_800B9FA4);
    D_80097350 = 3;
    for (i = 3 * 0x68; i >= 0; i -= 0x68) {
        ((Slot *)((u8 *)D_80095904 + i))->x4 = 0;
    }
}
