#include "common.h"
#include "m2c.h"

s32 func_80055A0C(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80060D58(void *, M2C_UNK);             /* extern */

void func_800605D8(void *arg0, M2C_UNK arg1) {
    M2C_FIELD(arg0, s32 *, 0x20) = func_80055A0C(0x18, 0x3F, 0);
    func_80060D58(arg0, arg1);
}
