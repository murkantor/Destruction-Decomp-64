#include "common.h"
#include "m2c.h"

M2C_UNK func_80056CC4(M2C_UNK, M2C_UNK, M2C_UNK);   /* extern */
extern s32 D_800B0B58;

void func_80057308(void) {
    D_800B0B58 = 0;
    func_80056CC4(0, 2, 0);
}

s32 func_80055930(M2C_UNK *);                       /* extern */
M2C_UNK func_80055BB0(s32);                         /* extern */
M2C_UNK func_80055BBC(s32, s32);                    /* extern */
extern s32 D_800B0B68;
extern M2C_UNK D_80125C48;

void func_80057334(s32 arg0) {
    s32 var_s0;

    if (D_800B0B68 != 0) {
        var_s0 = D_800B0B68;
    } else {
        var_s0 = func_80055930(&D_80125C48);
        func_80055BB0(var_s0);
        D_800B0B68 = var_s0;
    }
    func_80055BBC(var_s0, arg0);
}
