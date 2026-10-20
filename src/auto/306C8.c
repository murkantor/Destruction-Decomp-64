#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 x0; f32 x4; f32 x8; s32 xC; } Scale;
typedef struct { s32 type; Vec3f normal; u8 pad10[4]; Vec3f x14; u8 pad20[0x48 - 0x20]; } Face;
typedef struct { u16 start; u16 count; } Cell;
typedef struct { u8 pad[0x20]; u16 *x20; Cell *x24; u16 *x28; } Grid;
typedef struct {
    u8 pad[0x30];
    Grid grid;
} TrackC;
typedef struct { u8 pad[0xC]; TrackC *xC; } Track70;
typedef struct { u8 pad[0x70]; Track70 *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct { Vec3f r[3]; } Mtx3f;
typedef struct { u8 pad[0xA4]; Vec3f pos; Vec3f vel; u8 padBC[0xC]; Mtx3f mat; } Phys;
typedef struct { Scale scale; Vec3f n; f32 t; u8 pad[0x40]; } Hit;
typedef struct {
    u8 pad[0x1EC];
    Phys phys;
    u8 pad2D8[0xB90 - 0x2D8];
    f32 xB90;
    f32 xB94;
    f32 xB98;
} Obj;
typedef struct {
    u8 pad[0x2C];
    Vec3f x2C;
    u8 pad38[0xA4 - 0x38];
    Vec3f xA4;
    u8 padB0[4];
    s32 xB4;
} Body;
typedef struct {
    Body body;
    u8 padB8[0x120 - 0xB8];
    f32 x120;
    u8 pad124[0x17C - 0x124];
    f32 x17C;
    f32 x180;
    f32 x184;
    u8 pad188[0x194 - 0x188];
    Vec3f x194;
    f32 x1A0;
    f32 x1A4;
    s32 x1A8;
    u8 pad1AC[0x1B8 - 0x1AC];
    f32 x1B8;
} Car;

extern Game *gGame;
extern Vec3f Vec3f_TransformTransposed(Mtx3f *m, Vec3f *t, Vec3f *v);
extern s32 Track_GetCellIndex(f32 x, f32 z);
extern void Track_GetFace(Face *, s32);
extern u8 Poly_ContainsXZMargin(Face *p, Vec3f *pt, f32 m);
extern u8 Plane_IntersectSegment(Vec3f *p, Vec3f *n, f32 d, Vec3f *a, Vec3f *b, Vec3f *out1, Vec3f *out2, f32 *t);
extern void Surface_GetGrip(s32 type, Scale *s);
extern void Vec3f_Length(f32 *, Vec3f *);
extern s32 rand(void);
extern f32 sqrtf(f32);

void Wheel_ProbeGround(Obj *obj, Car *car) {
    Hit h;
    Vec3f p80;
    Vec3f p90;
    Vec3f pA0;
    Vec3f n;
    Face face;
    Vec3f o1;
    Vec3f o2;
    f32 t;
    f32 len;
    Grid *g;
    Body *b;
    Mtx3f *m;
    Phys *ph;
    s32 bestType;
    s32 idx;
    s32 i;
    s32 count;
    s32 start;
    Cell *cell;
    f32 mul;
    f32 f6;
    f32 tmp;
    f32 r;
    f32 lim;
    f32 k;
    f32 k1;
    f32 k2;
    f32 top;
    f32 lo;
    f32 hi;

    g = &gGame->x1100->x70->xC->grid;
    m = &obj->phys.mat;
    b = &car->body;
    ph = &obj->phys;
    h.t = 1.25f;
    PORT_CONVERT(&b->x2C, 3, 4); /* copied into a local */
    pA0 = b->x2C;
    pA0.y = 0.6f;
    p90 = Vec3f_TransformTransposed(m, &obj->phys.pos, &pA0);
    p80 = Vec3f_TransformTransposed(m, &obj->phys.pos, &b->xA4);
    idx = Track_GetCellIndex(p80.x, p80.z);
    bestType = -999;
    ph = &obj->phys;
    if (idx < 0) {
        return;
    }
    count = g->x24[g->x20[idx]].count;
    start = g->x24[g->x20[idx]].start;
    for (i = 0; i < count; i++) {
        Track_GetFace(&face, g->x28[start + i]);
        n = face.normal;
        if (n.x * m->r[1].x + n.y * m->r[1].y + n.z * m->r[1].z > 0.5 &&
            n.y > 0.707 &&
            Poly_ContainsXZMargin(&face, &p80, 0.3f) &&
            Plane_IntersectSegment(&face.x14, &n, car->x120, &p90, &p80, &o1, &o2, &t) &&
            t > 0.0f && t < h.t) {
            mul = (f32)(sqrtf(ph->vel.x * ph->vel.x + ph->vel.y * ph->vel.y + ph->vel.z * ph->vel.z) * 0.2777778) * (0.05 * 3) + 1.5;
            Surface_GetGrip(face.type, &h.scale);
            if (obj->xB90 != 1.0) {
                if (h.scale.x4 != 1.25) {
                    f6 = 1.25 - h.scale.x4;
                    k = obj->xB94 * f6;
                    h.scale.x4 = 1.25 - k;
                    if (h.scale.x4 < 0.5) {
                        h.scale.x4 = 0.5f;
                    }
                }
                if (h.scale.x8 != 1.25) {
                    f6 = 1.25 - h.scale.x8;
                    k = obj->xB94 * f6;
                    tmp = 1.0 - f6 * 1.6666666f;
                    car->x1B8 = tmp + obj->xB98 * (1.0 - tmp);
                    if (car->x1B8 > 1.0) {
                        car->x1B8 = 1.0f;
                    }
                    h.scale.x8 = 1.25 - k;
                    if (h.scale.x8 < 0.5) {
                        h.scale.x8 = 0.5f;
                    }
                } else {
                    car->x1B8 = 1.0f;
                }
            } else {
                car->x1B8 = 1.0f;
            }
            h.scale.x4 *= mul;
            h.scale.x8 *= mul;
            h.t = t;
            h.n = n;
            bestType = face.type;
        }
    }
    if (h.t < 1.25) {
        k1 = (h.t - 1.0f) * (b->xA4.y - pA0.y);
        car->x194 = h.n;
        car->x1A4 = h.scale.x4;
        car->x1A0 = h.scale.x8;
        car->x1A8 = bestType;
        b->xA4.y += k1;
        Vec3f_Length(&len, &ph->vel);
        lim = len * 0.003;
        if (lim > 0.1) {
            lim = 0.1f;
        }
        r = rand() * (1.0f / 32767.0f);
        if (r < 0.02 || r > 0.91) {
            b->xA4.y += lim * (r - 0.5);
        }
        b->xB4 = 0;
        top = car->x184;
        lo = car->x180;
        hi = car->x17C;
        if (b->xA4.y < top - lo) {
            b->xA4.y = top - lo;
        }
        if (top - hi < b->xA4.y) {
            b->xA4.y = top - hi;
        }
    }
}
