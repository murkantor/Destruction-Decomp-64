#include "common.h"
#include "m2c.h"

typedef struct {
    s32 a;
    s32 b;
    s32 c;
} UiTriple;

extern void *gUi[];
extern u8 D_801253F9;
extern s32 D_80097A14;
extern s32 D_80097350;
extern s32 D_800958B0[];
extern UiTriple D_80096780[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void func_8003ECF8(void);

void func_8003FC08(void) {
    s32 i;

    Ui_ResetScreen(0x112B4, 0x112B5);
    Ui_AddElement(gUi[0], 2, 0x3E);
    Ui_AddElement(gUi[0], 2, 0x6B);
    if (D_801253F9) {
        D_80097350 = 4;
    } else {
        D_80097350 = D_80097A14 + 1;
    }
    for (i = 0; i < D_80097350; i++) {
        Ui_AddElement(gUi[0], 2, D_80096780[i].a);
    }
    for (i = D_80097350; i < 4; i++) {
        Ui_AddElement(gUi[0], 2, D_80096780[i].c);
    }
    Ui_RemoveElement(gUi[0], D_80096780[D_800958B0[0]].a);
    Ui_AddElement(gUi[0], 2, D_80096780[D_800958B0[0]].b);
    func_8003ECF8();
}
