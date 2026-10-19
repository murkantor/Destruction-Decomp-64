#include "common.h"
#include "m2c.h"

f32 func_80066F24(void *arg0) {
    u8 temp_v0;

    temp_v0 = M2C_FIELD(arg0, u8 *, 0xC8) - 1;
    M2C_FIELD(arg0, u8 *, 0xC8) = temp_v0;
    if (!(temp_v0 & 0xFF)) {
        if (M2C_FIELD(arg0, s8 *, 0xC9) == 0) {
            M2C_FIELD(arg0, s8 *, 0xC9) = (s8) M2C_FIELD(arg0, u8 *, 0xD0);
            M2C_FIELD(arg0, u8 *, 0xC8) = (u8) M2C_FIELD(arg0, u8 *, 0xC6);
        } else {
            M2C_FIELD(arg0, s8 *, 0xC9) = 0;
            M2C_FIELD(arg0, u8 *, 0xC8) = (u8) M2C_FIELD(arg0, u8 *, 0xC7);
        }
    }
    return (f32) M2C_FIELD(arg0, s8 *, 0xC9);
}

f32 sinf(f32);                                      /* extern */
extern f64 D_800BBFB8;

f32 func_80066F7C(void *arg0) {
    s32 temp_v1;

    temp_v1 = M2C_FIELD(arg0, u16 *, 0xA2) - M2C_FIELD(arg0, u8 *, 0xAE);
    if (temp_v1 > 0) {
        M2C_FIELD(arg0, f32 *, 0x64) = (f32) (sinf((f32) ((f64) (2.0f * ((f32) temp_v1 / (f32) M2C_FIELD(arg0, u8 *, 0xCD))) * D_800BBFB8)) * M2C_FIELD(arg0, f32 *, 0x1C));
    }
    return M2C_FIELD(arg0, f32 *, 0x64);
}
