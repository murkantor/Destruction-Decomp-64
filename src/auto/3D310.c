#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10FC]; s16 x10FC; } Game;
typedef struct { u8 pad0[0x10]; s32 x10; } Obj3C710;
extern Game *gGame;
extern s16 D_8011EEEA;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
void func_800506E4(s32 *);
u8 func_80050910(s32);
void func_80063380(s32);
void func_80063564(s32, s32);
void Timer_Start(void *, s64);

s32 func_8003C710(Obj3C710 *obj) {
    s32 x;

    if (obj->x10 == 1) {
        func_80063564(8, 1);
        obj->x10 = 0;
    } else {
        if (Pak_Open(&x) && x == 5) {
            func_800506E4(&x);
            if (func_80050910(x)) {
                func_80063380(2);
                func_80063564(0x19, 1);
                obj->x10 = 0;
            }
        }
        if (gGame->x10FC == D_8011EEEA) {
            func_80063564(6, 1);
            obj->x10 = 0;
        }
    }
    Timer_Start(obj, 300000000);
    D_8009734C = 0;
    D_8009734D = 0;
    return 1;
}
