#include "common.h"
#include "m2c.h"

M2C_UNK Zip_ReadFileByName(M2C_UNK *, s32, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK D_800BB7D4;

void Zip_LoadFile(s32 arg0, M2C_UNK arg1, M2C_UNK arg2) {
    Zip_ReadFileByName(&D_800BB7D4, arg0, arg1, arg2);
}
