#include "common.h"
#include "m2c.h"

M2C_UNK func_80058A74();                            /* extern */
M2C_UNK func_80058B58();                            /* extern */
s32 func_800597DC(M2C_UNK, M2C_UNK *);              /* extern */
extern M2C_UNK D_80097D50;
extern u8 D_80097D58;

void func_80050684(s32 *arg0) {
    if (D_80097D58 == 0) {
        func_80058A74();
    }
    *arg0 = func_800597DC(0, &D_80097D50);
    if (D_80097D58 == 0) {
        func_80058B58();
    }
}
