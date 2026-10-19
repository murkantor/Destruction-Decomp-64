#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; s32 rest[11]; } UiElemDesc;

typedef struct {
    u8 pad0[0x1058];
    u64 x1058;
} Game;

extern Game *gGame;
extern void *gUi[];
extern u8 D_8009734C;
extern u64 D_800964D8;
extern char gSaveSlotText[][0x32];
extern UiElemDesc D_800BA068;
extern UiElemDesc D_800BA0B0;
extern char D_800BA0F8[];
extern char D_800BA114[];
extern char D_800BA128[];
extern char D_800BA13C[];
void Ui_ResetScreen(s32, s32);
void Ui_AddSpriteElement(void *, UiElemDesc *);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
u64 Time_Get(u64 *);

void PakMenu_Busy(void) {
    UiElemDesc a = D_800BA068;
    UiElemDesc b = D_800BA0B0;

    Ui_ResetScreen(0x11564, 0x11565);
    if (D_8009734C) {
        Ui_AddSpriteElement(gUi[0], &a);
        Ui_AddElement(gUi[0], 2, 0xB1);
    } else {
        Ui_AddSpriteElement(gUi[0], &b);
        Ui_AddElement(gUi[0], 2, 0xB2);
    }
    Ui_AddElement(gUi[0], 2, 0x131);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    sprintf(gSaveSlotText[0], D_800BA0F8);
    if (D_8009734C) {
        sprintf(gSaveSlotText[1], D_800BA114);
    } else {
        sprintf(gSaveSlotText[1], D_800BA128);
    }
    sprintf(gSaveSlotText[2], D_800BA13C);
    D_800964D8 = Time_Get(&gGame->x1058);
}
