#include "common.h"
#include "m2c.h"

typedef struct { s32 used; u8 pad4[0x1C]; } SaveSlot;
typedef struct { s32 x0; u8 pad4[0x64]; } Ent68;
extern void *gUi[];
extern char gSaveSlotText[];
extern char D_80123CBA[];
extern SaveSlot gSaveSlots[16];
extern s32 D_80097D80;
extern s32 D_80097350;
extern Ent68 D_80095908[];
extern char D_800BA178[];
extern char D_800BA198[];
extern char D_800BA1B4[];
extern char D_800BA1BC[];
extern char D_800BA1C8[];
extern char D_800BA1EC[];
extern char D_800BA1FC[];
extern char D_800BA210[];
extern char D_800BA218[];
extern char D_800BA220[];
void Ui_ResetScreen(s32, s32);
void Pak_RefreshSaveSlots(void);
void Ui_AddElement(void *, s32, s32);
int sprintf(char *, const char *, ...);

void PakMenu_PakFull(void) {
    s32 n = 0;
    s32 i;

    Ui_ResetScreen(0x112C8, 0x112C9);
    Pak_RefreshSaveSlots();
    Ui_AddElement(gUi[0], 2, 0x22);
    Ui_AddElement(gUi[0], 2, 0x21);
    Ui_AddElement(gUi[0], 2, 0x131);
    Ui_AddElement(gUi[0], 2, 0x141);
    Ui_AddElement(gUi[0], 2, 0x142);
    sprintf(gSaveSlotText, D_800BA178);
    sprintf(gSaveSlotText + 0x32, D_800BA198);
    sprintf(gSaveSlotText + 0x64, D_800BA1B4);
    sprintf(gSaveSlotText + 0x96, D_800BA1BC, 4);
    sprintf(gSaveSlotText + 0xC8, D_800BA1C8);
    for (i = 0; i < 16; i++) {
        n += gSaveSlots[i].used != 0;
    }
    sprintf(D_80123CBA, D_800BA1EC, n);
    sprintf(D_80123CBA + 0x32, D_800BA1FC, D_80097D80 / 256);
    sprintf(D_80123CBA + 0x64, D_800BA210);
    sprintf(D_80123CBA + 0x96, D_800BA218);
    sprintf(D_80123CBA + 0xC8, D_800BA220);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    Ui_AddElement(gUi[0], 0, 0x17F);
    Ui_AddElement(gUi[0], 0, 0x180);
    Ui_AddElement(gUi[0], 0, 0x181);
    Ui_AddElement(gUi[0], 0, 0x182);
    Ui_AddElement(gUi[0], 0, 0x183);
    D_80097350 = 3;
    for (i = 3 * sizeof(Ent68); i >= 0; i -= sizeof(Ent68)) {
        *(s32 *)((u8 *)D_80095908 + i) = 0;
    }
}
