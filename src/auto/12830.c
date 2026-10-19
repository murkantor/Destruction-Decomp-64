#include "common.h"
#include "m2c.h"

extern void *D_80126138;

void func_80011C30(void *arg0, s8 *arg1, s8 *arg2) {
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_a3;

    var_a0 = (s32) ((M2C_FIELD(arg0, f32 *, 0x2C) - M2C_FIELD(D_80126138, f32 *, 0x10)) / M2C_FIELD(D_80126138, f32 *, 0x18));
    var_a3 = (s32) ((M2C_FIELD(arg0, f32 *, 0x34) - M2C_FIELD(D_80126138, f32 *, 0x14)) / M2C_FIELD(D_80126138, f32 *, 0x1C));
    if (var_a0 < 0) {
        var_a0 = 0;
    } else {
        temp_v1 = M2C_FIELD(D_80126138, u8 *, 0) - 1;
        if (temp_v1 < var_a0) {
            var_a0 = temp_v1;
        }
    }
    if (var_a3 < 0) {
        var_a3 = 0;
    } else {
        temp_v1_2 = M2C_FIELD(D_80126138, u8 *, 1) - 1;
        if (temp_v1_2 < var_a3) {
            var_a3 = temp_v1_2;
        }
    }
    *arg1 = (s8) var_a0;
    *arg2 = (s8) var_a3;
}
