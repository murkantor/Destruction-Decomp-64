#include "common.h"
#include "m2c.h"

M2C_UNK func_8006A460(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK func_8006A930(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 D_800B6D30;
extern M2C_UNK D_800D1070;
extern M2C_UNK D_801255B0;

void func_80075150(void) {
    D_800B6D30 = 1;
    func_8006A460(&D_801255B0, &D_800D1070, 1);
    func_8006A930(&D_801255B0, 0, 0);
}
