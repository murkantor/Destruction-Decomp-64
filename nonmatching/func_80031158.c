typedef struct { f32 x, y, z; } Vec3f;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
double fabs(double);

s32 func_80031158(Vec3f *p1, Vec3f *d1, Vec3f *p2, Vec3f *d2, Vec3f *out1, Vec3f *out2) {
    Vec3f diff;
    f32 a, b, c;
    f32 denom, t;

    Vec3f_Sub(&diff, p1, p2);
    if (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z < 1e-12) {
        *out1 = *p1;
        *out2 = *p2;
        return 1;
    }
    Vec3f_Dot(&a, d1, d2);
    Vec3f_Dot(&b, &diff, d2);
    Vec3f_Dot(&c, &diff, d1);
    denom = a * a - 1.0f;
    c = c - b * a;
    if (fabs(denom) < 1e-6) {
        return 0;
    }
    c = c / denom;
    out1->x = p1->x + c * d1->x;
    out1->y = p1->y + c * d1->y;
    out1->z = p1->z + c * d1->z;
    t = b + c * a;
    out2->x = p2->x + t * d2->x;
    out2->y = p2->y + t * d2->y;
    out2->z = p2->z + t * d2->z;
    return 1;
}
