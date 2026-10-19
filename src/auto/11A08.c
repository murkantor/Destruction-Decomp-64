#include "common.h"
#include "m2c.h"

void func_80010E08(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0x38) = arg1;
}

void func_80010E10(void *arg0, s8 arg1, s8 arg2, s8 arg3) {
    M2C_FIELD(arg0, s8 *, 0x35) = arg1;
    M2C_FIELD(arg0, s8 *, 0x36) = arg2;
    M2C_FIELD(arg0, s8 *, 0x37) = arg3;
}

M2C_UNK Vec3f_Add(s32, s32, s32);               /* extern */

void func_80010E20(void *arg0) {
    s32 temp_a0;
    s32 temp_s0;
    u8 temp_a0_2;

    temp_s0 = arg0 + 0x10;
    Vec3f_Add(temp_s0, temp_s0, arg0 + 0x1C);
    temp_a0 = arg0 + 4;
    Vec3f_Add(temp_a0, temp_a0, temp_s0);
    temp_a0_2 = M2C_FIELD(arg0, u8 *, 0x30);
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (M2C_FIELD(arg0, f32 *, 0x28) + M2C_FIELD(arg0, f32 *, 0x2C));
    if ((temp_a0_2 & 0xFF) != 0xFF) {
        M2C_FIELD(arg0, u8 *, 0x30) = (u8) (temp_a0_2 - 1);
    }
}

void func_80010E94(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x28) = 0;
}
