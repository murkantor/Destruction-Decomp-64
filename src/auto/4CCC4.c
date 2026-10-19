#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame[];
extern u8 D_80097294[];
extern u8 D_800972A0[];
extern u8 D_800972A8[];
extern void func_80048B30(void *, s32, s32);
extern void Menu_LayoutResultRows(f32, void *, s32);
extern void Timer_Start(void *, s64);
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
s32 func_8004C0C4(Obj *o) {
    s32 ret = 0;
    s32 n = o->x10;
    if (n > 0) {
        o->x10 = n - 1;
        ret = 1;
        if (gGame[0]->x10FC == 0x22) {
            func_80048B30(D_80097294, 3, n);
        } else if (gGame[0]->x10FC == 0x23) {
            func_80048B30(D_80097294, 3, n);
        } else {
            func_80048B30(D_800972A0, 2, n);
        }
        Menu_LayoutResultRows(26.0f, D_800972A8, o->x10 + 1);
    }
    Timer_Start(o, 200000000);
    return ret;
}
