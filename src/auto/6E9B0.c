#include "common.h"
#include "m2c.h"

M2C_UNK func_8006A800(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8006DED4();                            /* extern */
s32 func_8006FD10(M2C_UNK, M2C_UNK *);              /* extern */
M2C_UNK func_80074B04();                            /* extern */
M2C_UNK func_80074B70();                            /* extern */
extern M2C_UNK D_800D3180;
extern u8 D_80125C42;

s32 func_8006DDB0(s32 arg0) {
    s32 temp_v0;

    func_80074B04();
    if (D_80125C42 != 1) {
        func_8006DED4();
        func_8006FD10(1, &D_800D3180);
        func_8006A800(arg0, 0, 1);
    }
    temp_v0 = func_8006FD10(0, &D_800D3180);
    D_80125C42 = 1;
    func_80074B70();
    return temp_v0;
}
