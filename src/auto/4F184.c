#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern s32 D_800958BC;
extern u8 D_80097335;
extern s32 D_800958A8;
extern s32 D_80097320[];
void func_80063380(s32);
void func_80063564(s32, s32);

s32 func_8004E584(Menu *m) {
    s32 i;

    if (D_800958BC == 3) {
        if (D_80097335) {
            m->x10 = 0;
            func_80063564(0x26, 1);
        } else {
            m->x10 = D_800958A8;
            func_80063564(0x13, 1);
        }
    } else {
        func_80063380(2);
        func_80063564(0x1A, 1);
    }
    {
        s32 *p;
        s32 v = -1;

        i = 11;
        p = D_80097320;
        for (; i >= 0; i--) {
            *p-- = v;
        }
    }
    return 1;
}
