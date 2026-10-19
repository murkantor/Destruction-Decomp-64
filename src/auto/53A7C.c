#include "common.h"
#include "m2c.h"

M2C_UNK func_80050D34(s32);                         /* extern */
M2C_UNK func_80055AB4(void *);                      /* extern */

void func_80052E7C(void *arg0) {
    s32 temp_a0;

    func_80055AB4(M2C_FIELD(arg0, void **, 0x4C));
    func_80055AB4(M2C_FIELD(arg0, void **, 0x14));
    func_80055AB4(M2C_FIELD(arg0, void **, 0xC));
    temp_a0 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_a0 != 0) {
        func_80050D34(temp_a0);
    }
    func_80055AB4(M2C_FIELD(arg0, void **, 0));
    func_80055AB4(arg0);
}

M2C_UNK func_80050D34(s32);                         /* extern */

void func_80052ED8(void *arg0) {
    s32 temp_a0;

    temp_a0 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_a0 != 0) {
        func_80050D34(temp_a0);
        M2C_FIELD(arg0, s32 *, 4) = 0;
    }
}

void func_80052F10(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x18) = 0;
}
