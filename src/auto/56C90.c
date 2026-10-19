#include "common.h"
#include "m2c.h"

void func_80056090(void) {

}

M2C_UNK func_8006C480();                            /* extern */
M2C_UNK func_8006C4D0(M2C_UNK);                     /* extern */

void func_80056098(void) {
    func_8006C480();
    func_8006C4D0(0);
}

s32 func_8006C120();                                /* extern */
s32 func_8006C160();                                /* extern */

void func_800560BC(void) {
    s32 temp_s0;

    do {
        temp_s0 = func_8006C120();
    } while (temp_s0 != func_8006C160());
}
