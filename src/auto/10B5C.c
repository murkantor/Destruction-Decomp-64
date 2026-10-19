#include "common.h"
#include "m2c.h"

void func_8000FF5C(void *arg0) {
    s32 (*temp_v0)(void *, s8);
    s32 (*temp_v0_2)(void *, s8);
    s32 var_v1;
    s32 var_v1_2;
    void *temp_a0;
    void *temp_a0_2;
    void *var_s0;
    void *var_s1;

    var_s1 = arg0;
    var_s0 = (void *)0x10000;
    var_s0 = var_s1 + 0x10000;
    var_s0 = M2C_FIELD(var_s1, void **, 0x8B54);
    var_v1 = 0;
    if (var_s0 != NULL) {
loop_1:
        temp_a0 = M2C_FIELD(var_s0, void **, 0);
        temp_v0 = M2C_FIELD(temp_a0, s32 (**)(void *, s8), 8);
        if (temp_v0 != NULL) {
            var_v1 = temp_v0(temp_a0, M2C_FIELD(temp_a0, s8 *, 0xC));
        }
        if (!(var_v1 & 0xFF)) {
            var_s0 = M2C_FIELD(var_s0, void **, 4);
            if (var_s0 != NULL) {
                goto loop_1;
            }
        }
    }
    var_s0 = (void *)0x10000;
    var_s0 = var_s1 + 0x10000;
    var_s0 = M2C_FIELD(var_s1, void **, 0x8B54);
    M2C_FIELD(var_s1, void **, 0x8B54) = (void *) M2C_FIELD(var_s1, void **, 0x8B6C);
    M2C_FIELD(var_s1, void **, 0x8B6C) = var_s0;
    var_s0 = (void *)0x10000;
    var_s0 = var_s1 + 0x10000;
    var_s0 = M2C_FIELD(var_s1, void **, 0x8B54);
    var_v1_2 = 0;
    if (var_s0 != NULL) {
loop_6:
        temp_a0_2 = M2C_FIELD(var_s0, void **, 0);
        temp_v0_2 = M2C_FIELD(temp_a0_2, s32 (**)(void *, s8), 8);
        if (temp_v0_2 != NULL) {
            var_v1_2 = temp_v0_2(temp_a0_2, M2C_FIELD(temp_a0_2, s8 *, 0xC));
        }
        if (!(var_v1_2 & 0xFF)) {
            var_s0 = M2C_FIELD(var_s0, void **, 4);
            if (var_s0 != NULL) {
                goto loop_6;
            }
        }
    }
    var_s0 = (void *)0x10000;
    var_s0 = var_s1 + 0x10000;
    var_s0 = M2C_FIELD(var_s1, void **, 0x8B54);
    M2C_FIELD(var_s1, void **, 0x8B54) = (void *) M2C_FIELD(var_s1, void **, 0x8B6C);
    M2C_FIELD(var_s1, void **, 0x8B6C) = var_s0;
}
