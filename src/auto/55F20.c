#include "common.h"
#include "m2c.h"

extern s8 D_800B0AE0;

s32 func_80055320(void) {
    D_800B0AE0 = 1;
    return 0;
}

M2C_UNK func_80055358();                            /* extern */

void func_80055334(void) {
    func_80055358();
    D_800B0AE0 = 0;
}

s32 func_80055358(void) {
    return 0;
}

/* Byte 7 is the low byte of the size word (size << 8 | pad); BE_OFF finds it on PC. */
void HeapBlock_Clear(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = (s32) M2C_FIELD(arg0, u8 *, BE_OFF8(7));
    M2C_FIELD(arg0, u8 *, BE_OFF8(7)) = 0U;
}

s32 HeapBlock_Ptr(s32 *arg0) {
    return *arg0;
}

u32 HeapBlock_Size(void *arg0) {
    return (u32) M2C_FIELD(arg0, u32 *, 4) >> 8;
}

u8 HeapBlock_Pad(void *arg0) {
    return M2C_FIELD(arg0, u8 *, BE_OFF8(7));
}

void Heap_Init(void *arg0, void *arg1, s32 arg2) {
    M2C_FIELD(arg1, s32 *, 0) = arg2;
    M2C_FIELD(arg1, s32 *, 4) = 0;
    M2C_FIELD(arg0, void **, 0) = arg1;
    M2C_FIELD(arg0, void **, 4) = arg1;
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    M2C_FIELD(arg0, s32 *, 0xC) = arg2;
    M2C_FIELD(arg0, s32 *, 0x14) = arg2;
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
}
