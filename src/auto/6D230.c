#include "common.h"
#include "m2c.h"

M2C_UNK func_8006A800(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8006C730();                            /* extern */
M2C_UNK func_8006C90C(M2C_UNK *, M2C_UNK);          /* extern */
M2C_UNK func_8006C9C0(M2C_UNK);                     /* extern */
s32 func_8006FD10(M2C_UNK, M2C_UNK *);              /* extern */
M2C_UNK func_80074B04();                            /* extern */
M2C_UNK func_80074B70();                            /* extern */
extern M2C_UNK D_800D3180;
extern u8 D_80125C42;

s32 func_8006C630(s32 arg0, M2C_UNK arg1) {
    M2C_UNK sp10;
    s32 temp_s0;

    func_80074B04();
    if (D_80125C42 != 0xFF) {
        func_8006C730();
        func_8006FD10(1, &D_800D3180);
        func_8006A800(arg0, 0, 1);
        func_8006FD10(0, &D_800D3180);
        func_8006A800(arg0, 0, 1);
        func_8006C9C0(0xFF);
        func_8006FD10(1, &D_800D3180);
        func_8006A800(arg0, 0, 1);
        D_80125C42 = 0xFF;
    }
    temp_s0 = func_8006FD10(0, &D_800D3180);
    func_8006A800(arg0, 0, 1);
    func_8006C90C(&sp10, arg1);
    func_80074B70();
    return temp_s0;
}
