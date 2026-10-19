#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
typedef struct {
    u8 pad[0xC];
    s16 mode;
} GameState;
extern GameState *gGame;
extern u8 D_80097335;
void Timer_Start(void *, s64);
void Menu_ShowVersus(s32, s32);

s32 func_8004AE34(MenuItem *item) {
    s32 n;
    s32 i;
    s32 max = 13;

    Timer_Start(item, 300000000LL);
    if (gGame->mode == 1) {
        n = D_80097335 ? 39 : 25;
    } else {
        n = (gGame->mode == 2) ? 31 : 25;
    }
    if (n == 25) {
        return 0;
    }
    i = item->value;
    if (i == max) {
        return 0;
    }
    Menu_ShowVersus(i, i + 1);
    item->value = i + 1;
    return 1;
}
