#include "common.h"
#include "m2c.h"

extern s32 D_800958A8[];
extern u8 D_80097335;
extern void Game_SetNextMode(s32, s32);
extern void func_80063380(s32);
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
s32 func_8004CA88(Obj *o) {
    if (D_800958A8[0] == 1) {
        if (D_80097335 != 0) {
            o->x10 = 0;
            Game_SetNextMode(0x26, 1);
        } else {
            func_80063380(2);
            Game_SetNextMode(0x19, 1);
            o->x10 = 0;
        }
    }
    if (D_800958A8[0] == 0) {
        Game_SetNextMode(0x24, 1);
    }
    if (D_800958A8[0] == 3) {
        o->x10 = 3;
        Game_SetNextMode(0x13, 1);
    }
    return 1;
}
