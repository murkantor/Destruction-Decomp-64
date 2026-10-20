#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x530]; u8 x530; } Hud;
extern Hud *D_8011EEF8;
extern u8 D_800958A0;
extern u8 D_80096ED0[12];
extern u8 D_80097354;
extern s32 D_80097358;
extern s32 D_80096EB8;
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 TitleScreen_Start(void) {
    s32 i;

    D_8011EEF8->x530 = 0;
    D_800958A0 = 1;
    for (i = 0; i < 12; i++) {
        if (i < 4) {
            D_80096ED0[i] = 1;
        } else {
            D_80096ED0[i] = 0;
        }
    }
    D_80097354 = 1;
    D_80097358 = 0;
    D_80096EB8 = 0;
    func_80063380(1);
    Game_SetNextMode(0x13, 1);
    return 1;
}
