#include "common.h"
#include "m2c.h"

s32 RomFile_Tell(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 8) - M2C_FIELD(arg0, s32 *, 0);
}

s32 RomFile_Close(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    return 0;
}

M2C_UNK func_8005314C(M2C_UNK *, s32, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK D_800BB7D4;

void func_80050D48(s32 arg0, M2C_UNK arg1, M2C_UNK arg2) {
    func_8005314C(&D_800BB7D4, arg0, arg1, arg2);
}
