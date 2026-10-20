#include "common.h"
#include "m2c.h"

f32 Vec3f_DistSq(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f4;

    temp_f2 = M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
    temp_f4 = M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
    temp_f0 = M2C_FIELD(arg0, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8);
    return (temp_f2 * temp_f2) + (temp_f4 * temp_f4) + (temp_f0 * temp_f0);
}
