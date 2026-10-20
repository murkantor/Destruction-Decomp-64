#include "common.h"
#include "m2c.h"

f32 Car_GetDriveTorque(void *arg0) {
    f32 temp_f2;
    f32 temp_f4;

    temp_f2 = (f32) M2C_FIELD(arg0, s32 *, 0xA54);
    temp_f4 = temp_f2 * temp_f2;
    return ((M2C_FIELD(arg0, f32 *, 0x1A0) * (temp_f4 * temp_f4)) + (M2C_FIELD(arg0, f32 *, 0x1A4) * (temp_f4 * temp_f2)) + (M2C_FIELD(arg0, f32 *, 0x1A8) * temp_f4) + M2C_FIELD(arg0, f32 *, 0x1AC)) * (M2C_FIELD(((M2C_FIELD(arg0, s32 *, 0xA58) * 4) + arg0), f32 *, 0x1B4) * M2C_FIELD(arg0, f32 *, 0x1CC) * 0.85f);
}
