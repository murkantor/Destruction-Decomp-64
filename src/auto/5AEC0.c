#include "common.h"
#include "m2c.h"

void Vec3f_Dot(f32 *arg0, void *arg1, void *arg2) {
    *arg0 = (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 4)) + (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 8));
}

void Vec3f_Cross(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 4)));
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 0)) - (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 8)));
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 0)));
}

void Vec3f_Add(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) + M2C_FIELD(arg2, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + M2C_FIELD(arg2, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) + M2C_FIELD(arg2, f32 *, 8));
}

void Vec3f_Sub(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) - M2C_FIELD(arg2, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) - M2C_FIELD(arg2, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) - M2C_FIELD(arg2, f32 *, 8));
}
