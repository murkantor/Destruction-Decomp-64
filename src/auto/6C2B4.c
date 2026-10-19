#include "common.h"
#include "m2c.h"

M2C_UNK func_8006B0BC(M2C_UNK, s32);                /* extern */
M2C_UNK func_8006B0DC(M2C_UNK);                     /* extern */

void func_8006B6B4(s32 arg0, M2C_UNK arg1) {
    func_8006B0DC(arg1);
    func_8006B0BC(arg1, arg0 + 0x14);
}
