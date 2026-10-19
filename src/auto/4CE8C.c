#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Unk4C28C;
extern s32 D_800958A8[];
extern u8 D_80097335;
void func_80063564(s32, s32);
void func_80063380(s32);

s32 func_8004C28C(Unk4C28C *arg0) {
    if (D_800958A8[0] == 1) {
        if (D_80097335) {
            arg0->x10 = 0;
            func_80063564(0x26, 1);
        } else {
            func_80063380(2);
            func_80063564(0x19, 1);
            arg0->x10 = 0;
        }
    }
    if (D_800958A8[0] == 0) {
        func_80063564(0x24, 1);
    }
    if (D_800958A8[0] == 3) {
        arg0->x10 = 3;
        func_80063564(0x13, 1);
    }
    return 1;
}
