#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Unk4C28C;
extern s32 D_800958A8[];
extern u8 D_80097335;
void Game_SetNextMode(s32, s32);
void func_80063380(s32);

s32 DerbyResults_Continue(Unk4C28C *arg0) {
    if (D_800958A8[0] == 1) {
        if (D_80097335) {
            arg0->x10 = 0;
            Game_SetNextMode(0x26, 1);
        } else {
            func_80063380(2);
            Game_SetNextMode(0x19, 1);
            arg0->x10 = 0;
        }
    }
    if (D_800958A8[0] == 0) {
        Game_SetNextMode(0x24, 1);
    }
    if (D_800958A8[0] == 3) {
        arg0->x10 = 3;
        Game_SetNextMode(0x13, 1);
    }
    return 1;
}
