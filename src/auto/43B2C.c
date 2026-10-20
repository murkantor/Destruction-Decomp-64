#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; s32 rest[11]; } UiElemDesc;
extern UiElemDesc D_800BA674;
extern UiElemDesc D_800BA6BC;
extern void *gUi[];
extern s32 D_800958A4;
extern s32 D_800958C0;
extern u8 D_801253F8;
extern u8 D_80097A0C[];
extern char D_80123CEC[];
extern char D_80096E4C[];
extern u8 D_80096E6E;
void Ui_ResetScreen(s32, s32);
void Ui_AddSpriteElement(void *, UiElemDesc *);
void func_8005D8B8(void *, s32, s32);
void Ui_AddElement(void *, s32, s32);
void CarSelect_SetCar(s32, s32, s32);
char *strcpy(char *, const char *);

void CarSelect_Show(void) {
    UiElemDesc a = D_800BA674;
    UiElemDesc b = D_800BA6BC;

    Ui_ResetScreen(0x112BA, 0x112BB);
    b.x14 = D_800958A4 + 0x11506;
    Ui_AddSpriteElement(gUi[0], &b);
    func_8005D8B8(gUi[0], 2, 0x3B);
    if (D_800958C0 == -1 || D_801253F8 != 0 || D_800958C0 < 12 || D_80097A0C[D_800958C0] != 0) {
        a.x14 = D_800958C0 + 0x11371;
    } else {
        a.x14 = 0x11389;
    }
    Ui_AddSpriteElement(gUi[0], &a);
    Ui_AddElement(gUi[0], 2, 0x3C);
    CarSelect_SetCar(D_800958C0, -1, 0);
    strcpy(D_80123CEC, D_80096E4C);
    D_80096E6E = 0;
}
