#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Foo;
extern u8 D_80097335;
extern s32 D_800958B0;
extern s32 D_800958B4;
extern s32 *D_80095CA8[];
void func_80063380(s32);
void func_80063564(s32, s32);
void func_8002D0E0(s32);

s32 func_8004B858(Foo *a) {
    s32 r;
    if (a->x10 == 0) {
        if (D_80097335 == 0) {
            func_80063380(2);
            func_80063564(0x1A, 1);
            D_800958B4 = *D_80095CA8[D_800958B0];
            func_8002D0E0(D_800958B0);
            r = 0;
        } else {
            func_80063564(0x26, 1);
            r = 0;
        }
    } else {
        if (D_80097335 == 0) {
            func_80063380(2);
            func_80063564(0x19, 1);
        } else {
            func_80063564(0x26, 1);
        }
        r = 0;
    }
    a->x10 = r;
    return 1;
}
