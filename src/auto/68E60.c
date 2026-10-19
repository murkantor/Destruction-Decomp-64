#include "common.h"
#include "m2c.h"

u32 func_8006BEA0();                                /* extern */
extern s32 D_800B4E54;

void func_80068260(void) {
    if (((func_8006BEA0() >> 2) == 0) && (D_800B4E54 == 0)) {
        D_800B4E54 = 0;
    }
}
