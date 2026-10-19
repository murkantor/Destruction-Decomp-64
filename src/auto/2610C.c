#include "common.h"
#include "m2c.h"

M2C_UNK func_80016090(s32);                         /* extern */
M2C_UNK func_80027ECC(void *);                      /* extern */
M2C_UNK func_80055AB4(void *);                      /* extern */

void func_8002550C(void *arg0) {
    s32 temp_a0;

    if (M2C_FIELD(arg0, s32 *, 0x20) != 0) {
        func_80027ECC(arg0 + 0x20);
    }
    if (M2C_FIELD(arg0, s32 *, 0x14) != 0) {
        func_80027ECC(arg0 + 0x14);
    }
    temp_a0 = M2C_FIELD(arg0, s32 *, 0xA8);
    if (temp_a0 != 0) {
        func_80016090(temp_a0);
    }
    func_80055AB4(arg0);
}
