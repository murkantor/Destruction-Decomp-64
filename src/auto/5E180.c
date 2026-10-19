#include "common.h"
#include "m2c.h"

M2C_UNK func_8000F980(s32);                         /* extern */
M2C_UNK func_8000FF5C();                            /* extern */
s32 func_8005E2AC(s32, M2C_UNK);                    /* extern */

void func_8005D580(s32 arg0, M2C_UNK arg1) {
    func_8000FF5C();
    func_8000F980(func_8005E2AC(arg0, arg1));
}

s16 func_80010214();                                /* extern */
s32 func_8001021C();                                /* extern */
s16 func_80064D70();                                /* extern */

void func_8005D5C4(void *arg0) {
    M2C_FIELD(arg0, s16 *, 0) = 0x3ED;
    M2C_FIELD(arg0, s16 *, 2) = func_80064D70();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s16 *, 0x8B66) = func_80010214();
    M2C_FIELD(arg0, s32 *, 0x8B58) = func_8001021C();
    M2C_FIELD(arg0, s32 *, 0x8B54) = 0;
    M2C_FIELD(arg0, s8 *, 0x8B68) = 0;
    M2C_FIELD(arg0, s32 *, 0x8B6C) = 0;
}

M2C_UNK func_8000FACC(s32);                         /* extern */
M2C_UNK Ctx_FreeAll();                            /* extern */

void func_8005D640(s32 arg0) {
    Ctx_FreeAll();
    func_8000FACC(arg0);
}
