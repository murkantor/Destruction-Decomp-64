#include "common.h"
#include "m2c.h"

M2C_UNK RomFile_Close(s32);                         /* extern */
M2C_UNK Mem_Free(void *);                      /* extern */

void func_80052E7C(void *arg0) {
    s32 temp_a0;

    Mem_Free(M2C_FIELD(arg0, void **, 0x4C));
    Mem_Free(M2C_FIELD(arg0, void **, 0x14));
    Mem_Free(M2C_FIELD(arg0, void **, 0xC));
    temp_a0 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_a0 != 0) {
        RomFile_Close(temp_a0);
    }
    Mem_Free(M2C_FIELD(arg0, void **, 0));
    Mem_Free(arg0);
}

M2C_UNK RomFile_Close(s32);                         /* extern */

void func_80052ED8(void *arg0) {
    s32 temp_a0;

    temp_a0 = M2C_FIELD(arg0, s32 *, 4);
    if (temp_a0 != 0) {
        RomFile_Close(temp_a0);
        M2C_FIELD(arg0, s32 *, 4) = 0;
    }
}

void func_80052F10(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x18) = 0;
}
