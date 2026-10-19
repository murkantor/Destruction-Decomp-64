#include "common.h"
#include "m2c.h"

f32 Math_Atan2(f32, f32);                        /* extern */

void Vec3f_AngleXY(f32 *arg0, void *arg1) {
    *arg0 = Math_Atan2(M2C_FIELD(arg1, f32 *, 4), M2C_FIELD(arg1, f32 *, 0));
}
