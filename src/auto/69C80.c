#include "common.h"
#include "m2c.h"

void guNormalize(f32 *arg0, f32 *arg1, f32 *arg2) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;

    temp_f4 = *arg0;
    temp_f2 = *arg1;
    temp_f0 = *arg2;
    temp_f2_2 = 1.0f / sqrtf((temp_f4 * temp_f4) + (temp_f2 * temp_f2) + (temp_f0 * temp_f0));
    *arg0 *= temp_f2_2;
    *arg1 *= temp_f2_2;
    *arg2 *= temp_f2_2;
}
