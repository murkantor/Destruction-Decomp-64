#include "common.h"
#include "m2c.h"

M2C_UNK func_80050ACC();                            /* extern */
M2C_UNK func_80063380(M2C_UNK);                     /* extern */
M2C_UNK func_80063564(M2C_UNK, M2C_UNK);            /* extern */

s32 func_8003D6F8(void) {
    func_80050ACC();
    func_80063380(2);
    func_80063564(0x1A, 1);
    return 1;
}

M2C_UNK PakMenu_InsertPaks();                            /* extern */
M2C_UNK func_8003EA7C(M2C_UNK, M2C_UNK);            /* extern */
M2C_UNK func_8003ECF8();                            /* extern */

void func_8003D72C(void) {
    func_8003ECF8();
    func_8003EA7C(0x112CC, 0x112CD);
    PakMenu_InsertPaks();
}
