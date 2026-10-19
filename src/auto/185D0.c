#include "common.h"
#include "m2c.h"

typedef struct Vec3f { f32 x, y, z; } Vec3f;

extern void Vec3f_Dot(f32 *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Sub(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);

s32 func_800179D0(Vec3f *out, Vec3f *in, s32 n, Vec3f *normal, f32 d)
{
    f32 dist[2];
    s32 i;
    s32 prev;
    s32 count;
    f32 t;

    Vec3f_Dot(&dist[0], normal, &in[n - 1]);
    dist[0] -= d;
    count = 0;
    for (i = 0; i < n; i++) {
        prev = i - 1;
        if (prev < 0)
            prev = n - 1;
        Vec3f_Dot(&dist[1], normal, &in[i]);
        dist[1] -= d;
        if (0.0f <= dist[0]) {
            out[count++] = in[prev];
        }
        if (dist[0] * dist[1] < 0.0f) {
            t = dist[0] / (dist[0] - dist[1]);
            Vec3f_Sub(&out[count], &in[i], &in[prev]);
            out[count].x *= t;
            out[count].y *= t;
            out[count].z *= t;
            Vec3f_Add(&out[count], &out[count], &in[prev]);
            count++;
        }
        dist[0] = dist[1];
    }
    return count;
}
