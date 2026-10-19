#include "common.h"
#include "m2c.h"

extern u8 D_800D1BAC;

s32 func_8004496C(void) {
    u8 *flag = &D_800D1BAC;

    if (*flag != 0) {
        return 0;
    }
    *flag = 1;
    return 1;
}
