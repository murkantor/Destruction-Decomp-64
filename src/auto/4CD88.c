#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } UnkObj;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
void Timer_Start(void *, s64);
void func_80048B30(void *, s32, s32);
void Menu_LayoutResultRows(f32, void *, s32);
extern s32 D_800958B4;
extern s32 D_80096038[];
extern Game *gGame[];
extern u8 D_80097294[];
extern u8 D_800972A0[];
extern u8 D_800972A8[];

s32 Menu_IncrementCount(UnkObj *o) {
    s32 ret = 0;
    s32 lim = D_80096038[D_800958B4];
    s32 n;
    s32 cur = o->x10;
    if (lim < 7 ? cur < lim : cur < 6) {
        n = o->x10++;
        ret = 1;
        if (gGame[0]->x10FC == 0x22) {
            func_80048B30(D_80097294, 3, n + 2);
        } else if (gGame[0]->x10FC == 0x23) {
            func_80048B30(D_80097294, 3, n + 2);
        } else {
            func_80048B30(D_800972A0, 2, n + 2);
        }
        Menu_LayoutResultRows(26.0f, D_800972A8, o->x10 + 1);
    }
    Timer_Start(o, 200000000);
    return ret;
}
