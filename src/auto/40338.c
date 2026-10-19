#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern s32 D_80097350;
extern s32 D_800958A4;
extern s32 D_800958A8[];
extern s32 D_80096750[][3];
extern char gSaveSlotText[][0x32];
extern char D_80096E2C[];
extern u8 D_80097335[];
extern u8 D_80097336;
extern f64 D_800BA390, D_800BA398;
void func_8003EA7C(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Ui_SetElementX(void *, s32, s32, f32);
char *strcpy(char *, const char *);
s32 Font_GetStringWidth(char *, s32);
void func_8002D3B8(s32);
void func_8003ECF8(void);

void PakMenu_ShowSaveSlots(void) {
    s32 i;
    func_8003EA7C(0x112B2, 0x112B3);
    D_80097350 = 4;
    for (i = 0; i < D_80097350; i++) {
        Ui_AddElement(gUi[0], 2, D_80096750[i][0]);
    }
    Ui_RemoveElement(gUi[0], D_80096750[D_800958A8[0]][0]);
    Ui_AddElement(gUi[0], 2, D_80096750[D_800958A8[0]][1]);
    strcpy(gSaveSlotText[0], D_80096E2C);
    Ui_SetElementX(gUi[0], 0, 0x17A, D_800BA398 - (f32)Font_GetStringWidth(gSaveSlotText[0], 0x111F4) * D_800BA390);
    func_8002D3B8(0);
    func_8002D3B8(1);
    func_8002D3B8(2);
    func_8002D3B8(3);
    func_8003ECF8();
    if (D_80097335[0]) D_80097335[0] = 0;
    if (D_80097336) D_80097336 = 0;
    D_800958A4 = 4;
}
