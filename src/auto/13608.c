#include "common.h"
#include "m2c.h"

extern s32 D_800D1C18;

s8 Surface_GetType(s32 arg0) {
    return M2C_FIELD(((arg0 * 4) + D_800D1C18), s8 *, 3);
}

void *Mem_Alloc(M2C_UNK);                           /* extern */

void func_80012A20(void **arg0) {
    void *temp_v0;

    temp_v0 = Mem_Alloc(0x2C);
    *arg0 = temp_v0;
    M2C_FIELD(temp_v0, s32 *, 0x10) = 0;
    M2C_FIELD(*arg0, void **, 0xC) = Mem_Alloc(0x15E0);
    M2C_FIELD(*arg0, s32 *, 0x18) = 0;
    M2C_FIELD(*arg0, void **, 0x14) = Mem_Alloc(0x15E0);
    M2C_FIELD(*arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(*arg0, s32 *, 0x20) = -1;
    M2C_FIELD(*arg0, s32 *, 0x24) = 0;
    M2C_FIELD(*arg0, s32 *, 0x28) = -1;
}
