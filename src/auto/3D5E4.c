#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Obj3C9E4;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game3C9E4;
extern Game3C9E4 *gGame;
extern s16 D_8011EEEA;
extern u8 D_800972F0;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
u8 func_80050910(s32);
void func_80063564(s32, s32);
void func_80063380(s32);
void Timer_Start(void *, s64);

s32 func_8003C9E4(Obj3C9E4 *arg0) {
    s32 sp10;

    if (arg0->x10 == 0) {
        func_80063564(5, 1);
    } else {
        if (Pak_Open(&sp10) && func_80050910(sp10)) {
            func_80063564(0xB, 1);
        }
        if (gGame->x10FC == D_8011EEEA || D_8011EEEA == 7 || D_8011EEEA == 0xC) {
            if (D_800972F0) {
                func_80063564(0x2E, 1);
            } else {
                func_80063380(2);
                func_80063564(0x19, 1);
                arg0->x10 = 0;
            }
            D_8009734C = 0;
            D_8009734D = 0;
        }
    }
    Timer_Start(arg0, 300000000);
    return 1;
}
