typedef struct { f32 x, y, z; } Vec3f;
void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
void Vec3f_Sub(Vec3f *out, Vec3f *a, Vec3f *b);
void Vec3f_Scale(Vec3f *out, Vec3f *a, f32 s);
f32 sqrtf(f32);
double fabs(double);
extern const double D_800B9600;
extern const double D_800B9608;
extern const double D_800B9610;
extern const double D_800B9618;

s32 Seg_SphereSweepIntersect(Vec3f *p0, Vec3f *p1, Vec3f *q0, Vec3f *q1, f32 r, f32 tmin, f32 tmax, f32 *outT, Vec3f *outA, Vec3f *outB, Vec3f *outMid)
{
    Vec3f tmp;
    f32 dpx, dpy, dpz, dqx, dqy, dqz, wx, wy, wz, dvx, dvy, dvz;
    f32 a, b, c, t, disc;

    dpx = p1->x - p0->x;
    dpy = p1->y - p0->y;
    dpz = p1->z - p0->z;
    dqx = q1->x - q0->x;
    dqy = q1->y - q0->y;
    dqz = q1->z - q0->z;
    wx = p0->x - q0->x;
    wy = p0->y - q0->y;
    wz = p0->z - q0->z;
    dvx = dpx - dqx;
    dvy = dpy - dqy;
    dvz = dpz - dqz;
    a = dvx * dvx + dvy * dvy + dvz * dvz;
    b = 2.0 * (wx * dvx + wy * dvy + wz * dvz);
    c = wx * wx + wy * wy + wz * wz;
    if (fabs(a) < D_800B9600) {
        return 0;
    }
    t = (b / a) * D_800B9608;
    if (r * (r * D_800B9610) < a * t * t + b * t + c) {
        return 0;
    }
    c = c - r * (r * D_800B9610);
    disc = b * b - a * (c * D_800B9610);
    if (disc < 0.0) {
        return 0;
    }
    if (a == 0.0) {
        return 0;
    }
    if (disc == 0.0) {
        t = -b / (2.0 * a);
    } else {
        f32 inv, q, base;
        disc = sqrtf(disc);
        inv = D_800B9618 / a;
        q = disc * inv;
        base = -b * inv;
        if (0.0 < q) {
            t = base - q;
        } else {
            t = base + q;
        }
    }
    if (t < tmin || tmax < t) {
        return 0;
    }
    Vec3f_Sub(&tmp, p1, p0);
    Vec3f_Scale(&tmp, &tmp, t);
    Vec3f_Add(outA, p0, &tmp);
    Vec3f_Sub(&tmp, q1, q0);
    Vec3f_Scale(&tmp, &tmp, t);
    Vec3f_Add(outB, q0, &tmp);
    Vec3f_Sub(&tmp, outB, outA);
    Vec3f_Scale(&tmp, &tmp, 0.5f);
    Vec3f_Add(outMid, outA, &tmp);
    *outT = t;
    return 1;
}
/* agent45: compile with --flags -ffast-math (inline sqrt.s). Then only b (f12 vs f10) and the D_800B9610 constant pseudo (f10 vs f12) are swapped: global-alloc priority of const (3 refs/14 insns, DF) beats b (7 refs/47). */
