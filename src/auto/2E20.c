#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */
s32 func_80063A20(s32);                             /* extern */
extern void *D_8011EEE4;

void func_80002220(void *arg0) {
    s32 temp_a0;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = Mem_Alloc(0x10);
    temp_v0 = Mem_Alloc(0x20);
    M2C_FIELD(temp_v0, s16 *, 0) = 8;
    M2C_FIELD(temp_v0, void **, 0x1C) = temp_s0;
    M2C_FIELD(temp_v0, s32 *, 0xC) = (s32) M2C_FIELD(arg0, s32 *, 4);
    M2C_FIELD(temp_v0, s32 *, 0x14) = (s32) M2C_FIELD(arg0, s32 *, 8);
    M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x1C), f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0xC);
    M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x1C), f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x10);
    M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x1C), f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x14);
    M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x1C), f32 *, 0xC) = (f32) M2C_FIELD(arg0, f32 *, 0x18);
    M2C_FIELD(M2C_FIELD(D_8011EEE4, void **, 0x1100), void **, 0x2C) = temp_v0;
    temp_a0 = M2C_FIELD(arg0, s32 *, 0x1C);
    if (temp_a0 == 0) {
        M2C_FIELD(temp_v0, s32 *, 0x10) = 0;
    } else {
        M2C_FIELD(temp_v0, s32 *, 0x10) = func_80063A20(temp_a0);
    }
    M2C_FIELD(temp_v0, s32 *, 0x18) = func_80063A20(M2C_FIELD(arg0, s32 *, 0x20));
    M2C_FIELD(M2C_FIELD(D_8011EEE4, void **, 0x1100), void **, 0x2C) = temp_v0;
}
