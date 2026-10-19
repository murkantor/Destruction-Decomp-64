#include "common.h"
#include "m2c.h"

M2C_UNK func_8006B050(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
extern M2C_UNK D_800BBB6C;
extern M2C_UNK D_800BBB70;

void func_80054CD0(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    if (arg0 == NULL) {
        func_8006B050(&D_800BBB6C, &D_800BBB70, 0x4C);
    }
    if (arg3 == 0) {
        func_8006B050(&D_800BBB6C, &D_800BBB70, 0x4D);
    }
    if (arg4 == 0) {
        func_8006B050(&D_800BBB6C, &D_800BBB70, 0x4E);
    }
    if (arg5 == 0) {
        func_8006B050(&D_800BBB6C, &D_800BBB70, 0x4F);
    }
    M2C_FIELD(arg0, s8 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = arg1;
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    M2C_FIELD(arg0, s32 *, 0x10) = arg3;
    M2C_FIELD(arg0, s32 *, 0xC) = arg4;
    M2C_FIELD(arg0, s32 *, 0x14) = arg5;
    M2C_FIELD(arg0, s32 *, 0x18) = arg6;
}
