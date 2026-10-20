#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; u32 x10; } Foo;
extern s32 D_800958AC[];
extern u32 D_800958B4;
void Game_SetNextMode(s32, s32);

s32 GameTypeMenu_Select(Foo *a) {
    u32 v;
    D_800958AC[0] = a->x10;
    Game_SetNextMode(0x17, 1);
    if (D_800958AC[0] == 3) {
        if (D_800958B4 < 4) {
            D_800958B4 = 4;
            v = 0;
        } else {
            v = D_800958B4 - 4;
        }
    } else {
        v = D_800958B4;
        if (v >= 4) {
            v = 0;
            D_800958B4 = 0;
        }
    }
    a->x10 = v;
    return 1;
}
