#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad0[4]; Vec3f n; u8 pad10[0x48 - 0x10]; } Face;
typedef struct { u8 pad[0x290]; Vec3f x290; } Body;
typedef struct { u8 pad[0x20]; Body *body; } Res;
typedef struct {
    u8 pad0[0x38];
    Vec3f pos;
    u8 pad44[0x54 - 0x44];
    Vec3f prev;
    u8 pad60[0xB0 - 0x60];
    Res *res;
    u8 xB4;
} Car;
typedef struct { s32 x0; s32 x4; } Ghost;

extern u8 D_80095628;
extern Ghost *D_8009562C;
extern f64 D_800B8C30, D_800B8C38, D_800B8C40, D_800B8C48, D_800B8C50, D_800B8C58, D_800B8C60, D_800B8C68, D_800B8C70;
extern f64 fabs(f64);
extern void Vec3f_Sub(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Add(Vec3f *out, Vec3f *a, Vec3f *b);
extern void Vec3f_Length(f32 *out, Vec3f *v);
extern void Vec3f_Normalize(Vec3f *out, Vec3f *v);
extern void Vec3f_Scale(Vec3f *out, Vec3f *v, f32 s);
extern void Vec3f_Cross(Vec3f *out, Vec3f *a, Vec3f *b);
extern s32 Track_GetCellsInBox(Vec3f *a, Vec3f *b, s32 *cells, s32 max);
extern void Track_GetFace(Face *face, s32 cell);
extern u8 func_80034794(Face *face, f32 r, Vec3f *a, Vec3f *b, Vec3f *hit, void *x, f32 *t);
extern u8 Math_SolveQuadratic(f32 a, f32 b, f32 c, f32 *r0, f32 *r1);

void Car_ResolveTrackCollision(Car *car)
{
    Vec3f delta;
    Vec3f a;
    Vec3f b;
    s32 cells[0x400];
    Vec3f newPos;
    Vec3f diff;
    Vec3f faceN;
    f32 len;
    f32 t;
    f32 r0;
    f32 r1;
    Vec3f *target;
    s32 iter;
    s32 n;
    s32 i;
    f32 tmin;
    f32 px, py, pz, ax, ay, az;
    f32 dx, dy, dz;

    target = &car->res->body->x290;
    if (D_80095628 && car->xB4 == 9 && D_8009562C && D_8009562C->x0 == 0 && D_8009562C->x4 == 0) {
        return;
    }
    if (car->xB4 == 0) {
        return;
    }
    for (iter = 0; iter < 8; iter++) {
        a = car->prev;
        b = car->pos;
        Vec3f_Sub(&delta, &a, &b);
        Vec3f_Length(&len, &delta);
        tmin = 1.02f;
        if (len < D_800B8C30) {
            return;
        }
        if (len > D_800B8C38) {
            Vec3f norm;
            Vec3f scaled;

            Vec3f_Normalize(&norm, &delta);
            Vec3f_Scale(&scaled, &norm, -20.0f);
            Vec3f_Add(&b, &a, &scaled);
        }
        n = Track_GetCellsInBox(&a, &b, cells, 0x400);
        {
        Face face;
        Vec3f v;
        u8 x1100[0x10];
        for (i = 0; i < n; i++) {
            t = -1.0f;
            Track_GetFace(&face, cells[i]);
            if (func_80034794(&face, 0.25f, &a, &b, &v, x1100, &t)) {
                if (t < tmin && t > 0.0) {
                    tmin = t;
                    faceN = face.n;
                }
            }
        }
        if (tmin > D_800B8C40) {
            return;
        }
        tmin = tmin * D_800B8C48;
        newPos.x = (1.0f - tmin) * a.x + tmin * b.x;
        newPos.y = (1.0f - tmin) * a.y + tmin * b.y;
        newPos.z = (1.0f - tmin) * a.z + tmin * b.z;
        car->pos = newPos;
        Vec3f_Sub(&diff, target, &newPos);
        Vec3f_Length(&len, &diff);
        if (len >= D_800B8C50) {
            return;
        }
        if (fabs(faceN.y) > D_800B8C58) {
            v.x = a.z - b.z;
            v.y = 0.0f;
            v.z = a.x - b.x;
            Vec3f_Normalize(&v, &v);
        } else {
            v.x = faceN.z;
            v.y = 0.0f;
            v.z = faceN.x;
            Vec3f_Normalize(&v, &v);
        }
        {
        Vec3f axis;
        Vec3f_Cross(&axis, &faceN, &v);
        Vec3f_Normalize(&axis, &axis);
        Vec3f_Length(&len, &axis);
        if (len == 0.0) {
            return;
        }
        if (axis.y < 0.0) {
            Vec3f_Scale(&axis, &axis, -1.0f);
        }
        px = newPos.x;
        ax = axis.x;
        py = newPos.y;
        ay = axis.y;
        pz = newPos.z;
        az = axis.z;
        dx = target->x - px;
        dy = target->y - py;
        dz = target->z - pz;
        if (!Math_SolveQuadratic(ax * ax + ay * ay + az * az,
                                 dx * (ax * D_800B8C60) - 2.0 * dy * ay - 2.0 * dz * az,
                                 dx * dx + dy * dy + dz * dz - 2.8f * 2.8f, &r0, &r1)) {
            return;
        }
        r0 = r0 * D_800B8C68;
        r1 = r1 * D_800B8C68;
        if (r0 < 0.0) {
            r0 = r1;
        }
        if (r1 < 0.0) {
            r1 = r0;
        }
        if (r1 < r0) {
            r0 = r1;
        }
        if (fabs(r0) > D_800B8C70) {
            return;
        }
        car->pos.x = px + r0 * ax;
        car->pos.y = py + r0 * ay;
        car->pos.z = pz + r0 * az;
        }
        }
    }
}
