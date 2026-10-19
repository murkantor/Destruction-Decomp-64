#include "common.h"
#include "m2c.h"

s32 func_80055A0C(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80060C0C(void *);                      /* extern */

void func_80060550(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x18) = func_80055A0C(0x18, 0x3F, 0);
    func_80060C0C(arg0);
}
