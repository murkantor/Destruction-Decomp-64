#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10A8]; u64 x10A8; u8 pad10B0[0x4C]; s16 x10FC; } Game;
extern Game *gGame;
extern s16 D_8011EEEA;
extern void *gUi;
extern u8 D_80097354;
extern s32 D_80097358;
extern u8 D_80097334;
extern u8 D_8009734C;
extern u8 D_8009734D;
void func_8000E960(void);
void func_8005DC68(void *);
void func_80063380(s32);
void func_80063564(s32, s32);

s32 func_8003B508(void) {
    s32 ret = 0;
    u64 lim;

    if (gGame->x10FC != D_8011EEEA) {
        return 0;
    }
    lim = 6000000000ULL;
    if (gGame->x10A8 >= lim) {
        func_8000E960();
        func_8005DC68(gUi);
        D_80097354 = 1;
        D_80097358 = 0;
        D_80097334 = 1;
        D_8009734C = 1;
        D_8009734D = 0;
        func_80063380(1);
        func_80063564(5, 1);
        ret = 1;
    }
    return ret;
}
