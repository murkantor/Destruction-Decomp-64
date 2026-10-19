#include "common.h"
#include "m2c.h"

s32 Mem_AllocAligned(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80060C3C(void *, M2C_UNK);             /* extern */

void func_8006058C(void *arg0, M2C_UNK arg1) {
    M2C_FIELD(arg0, s32 *, 0x14) = Mem_AllocAligned(0x18, 0x3F, 0);
    func_80060C3C(arg0, arg1);
}
