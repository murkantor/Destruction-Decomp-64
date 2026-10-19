#include "common.h"
#include "m2c.h"

extern s32 D_800958A8[];
extern s32 D_800958AC;
extern s32 D_800958B0;
extern s32 D_800958B4;
extern s32 D_80125BE4[];
extern s32 D_80096EA0;
extern void Timer_Start(void *, s64);
extern void func_80063564(s32, s32);
extern void Ui_RemapTextures(void);
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
void Menu_SelectNext(Obj *o) {
    s32 next;
    Timer_Start(o, 300000000);
    next = -1;
    switch ((u32)D_800958A8[0]) {
    case 1:
        goto zero;
    case 0:
    zero:
        func_80063564(0x14, 1);
        next = D_800958B0;
        break;
    case 2:
        func_80063564(0x13, 1);
        next = D_800958A8[0];
        break;
    case 3:
        func_80063564(0x15, 1);
        next = D_800958AC;
        break;
    }
    D_800958B4 = D_80125BE4[o->x10];
    Ui_RemapTextures();
    D_80096EA0 = 1;
    o->x10 = next;
}
