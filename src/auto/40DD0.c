#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s8 x10; } Obj28;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad2[0x14]; Obj28 *x28; } Arg;
extern s32 gControllers, D_80125D30, D_80125DE0, D_80125E90;
extern s32 D_800958A4, D_800958AC;
extern void func_80063564(s32, s32);

s32 func_800401D0(Arg *arg) {
    D_800958A4 = arg->x10 + 2;
    switch (D_800958A4) {
    case 2:
        if (gControllers != 1 || D_80125D30 != 1) {
            goto fail;
        }
        if (arg->x28->x10 != 2 && arg->x28->x10 != 3) {
            break;
        }
        goto fail;
    case 3:
        if (gControllers != 1 || D_80125D30 != 1 || D_80125DE0 != 1) {
            goto fail;
        }
        if (arg->x28->x10 == 3) {
            goto fail;
        }
        if (D_800958AC == 2 || D_800958AC == 3) {
            D_800958AC = 0;
        }
        break;
    case 4:
        if (gControllers != 1 || D_80125D30 != 1 || D_80125DE0 != 1 || D_80125E90 != 1) {
        fail:
            D_800958A4 = 4;
            return 0;
        }
        if (D_800958AC == 3) {
            D_800958AC = 0;
        }
        break;
    }
    func_80063564(0x15, 1);
    arg->x10 = D_800958AC;
    return 1;
}
