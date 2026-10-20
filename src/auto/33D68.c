#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
f64 fabs(f64);
f32 fabsf(f32);

/* Separating-axis test for two oriented boxes (extents, 3x3 axes, centre). Returns 1 if they overlap. */
u8 Obb_Overlap(const f32 *ea, const f32 *ua, Vec3f *ca, const f32 *eb, const f32 *ub, Vec3f *cb) {
    f32 ra, rb;
    f32 d;
    f32 R00, R01, R02, R10, R11, R12, R20, R21, R22;
    f32 t[3];
    Vec3f T;

    R00 = ua[0] * ub[0] + ua[1] * ub[1] + ua[2] * ub[2];
    R01 = ua[0] * ub[3] + ua[1] * ub[4] + ua[2] * ub[5];
    R02 = ua[0] * ub[6] + ua[1] * ub[7] + ua[2] * ub[8];
    R10 = ua[3] * ub[0] + ua[4] * ub[1] + ua[5] * ub[2];
    R11 = ua[3] * ub[3] + ua[4] * ub[4] + ua[5] * ub[5];
    R12 = ua[3] * ub[6] + ua[4] * ub[7] + ua[5] * ub[8];
    R20 = ua[6] * ub[0] + ua[7] * ub[1] + ua[8] * ub[2];
    R21 = ua[6] * ub[3] + ua[7] * ub[4] + ua[8] * ub[5];
    R22 = ua[6] * ub[6] + ua[7] * ub[7] + ua[8] * ub[8];
    T.x = cb->x - ca->x;
    T.y = cb->y - ca->y;
    T.z = cb->z - ca->z;
    t[0] = T.x * ua[0] + T.y * ua[1] + T.z * ua[2];
    t[1] = T.x * ua[3] + T.y * ua[4] + T.z * ua[5];
    t[2] = T.x * ua[6] + T.y * ua[7] + T.z * ua[8];

    ra = ea[0];
    rb = eb[0] * fabs(R00) + eb[1] * fabs(R01) + eb[2] * fabs(R02);
    d = fabsf(t[0]);
    if (d > ra + rb) return 0;
    ra = ea[1];
    rb = eb[0] * fabs(R10) + eb[1] * fabs(R11) + eb[2] * fabs(R12);
    d = fabsf(t[1]);
    if (d > ra + rb) return 0;
    ra = ea[2];
    rb = eb[0] * fabs(R20) + eb[1] * fabs(R21) + eb[2] * fabs(R22);
    d = fabsf(t[2]);
    if (d > ra + rb) return 0;

    ra = ea[0] * fabs(R00) + ea[1] * fabs(R10) + ea[2] * fabs(R20);
    rb = eb[0];
    d = fabsf(t[0] * R00 + t[1] * R10 + t[2] * R20);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R01) + ea[1] * fabs(R11) + ea[2] * fabs(R21);
    rb = eb[1];
    d = fabsf(t[0] * R01 + t[1] * R11 + t[2] * R21);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R02) + ea[1] * fabs(R12) + ea[2] * fabs(R22);
    rb = eb[2];
    d = fabsf(t[0] * R02 + t[1] * R12 + t[2] * R22);
    if (d > ra + rb) return 0;

    ra = ea[1] * fabs(R20) + ea[2] * fabs(R10);
    rb = eb[1] * fabs(R02) + eb[2] * fabs(R01);
    d = fabsf(t[2] * R10 - t[1] * R20);
    if (d > ra + rb) return 0;
    ra = ea[1] * fabs(R21) + ea[2] * fabs(R11);
    rb = eb[0] * fabs(R02) + eb[2] * fabs(R00);
    d = fabsf(t[2] * R11 - t[1] * R21);
    if (d > ra + rb) return 0;
    ra = ea[1] * fabs(R22) + ea[2] * fabs(R12);
    rb = eb[0] * fabs(R01) + eb[1] * fabs(R00);
    d = fabsf(t[2] * R12 - t[1] * R22);
    if (d > ra + rb) return 0;

    ra = ea[0] * fabs(R20) + ea[2] * fabs(R00);
    rb = eb[1] * fabs(R12) + eb[2] * fabs(R11);
    d = fabsf(t[0] * R20 - t[2] * R00);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R21) + ea[2] * fabs(R01);
    rb = eb[0] * fabs(R12) + eb[2] * fabs(R10);
    d = fabsf(t[0] * R21 - t[2] * R01);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R22) + ea[2] * fabs(R02);
    rb = eb[0] * fabs(R11) + eb[1] * fabs(R10);
    d = fabsf(t[0] * R22 - t[2] * R02);
    if (d > ra + rb) return 0;

    ra = ea[0] * fabs(R10) + ea[1] * fabs(R00);
    rb = eb[1] * fabs(R22) + eb[2] * fabs(R21);
    d = fabsf(t[1] * R00 - t[0] * R10);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R11) + ea[1] * fabs(R01);
    rb = eb[0] * fabs(R22) + eb[2] * fabs(R20);
    d = fabsf(t[1] * R01 - t[0] * R11);
    if (d > ra + rb) return 0;
    ra = ea[0] * fabs(R12) + ea[1] * fabs(R02);
    rb = eb[0] * fabs(R21) + eb[1] * fabs(R20);
    d = fabsf(t[1] * R02 - t[0] * R12);
    if (d > ra + rb) return 0;
    return 1;
}
