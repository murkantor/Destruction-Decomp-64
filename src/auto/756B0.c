#include "common.h"
#include "m2c.h"

M2C_UNK func_8006A460(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK func_8006A930(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 D_800B6C30;
extern M2C_UNK D_800D1060;
extern M2C_UNK D_80131F10;

void func_80074AB0(void) {
    D_800B6C30 = 1;
    func_8006A460(&D_80131F10, &D_800D1060, 1);
    func_8006A930(&D_80131F10, 0, 0);
}
