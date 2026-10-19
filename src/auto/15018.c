#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    Vec3f pos;
    u8 padC[4];
    f32 x10;
    u8 pad14[8];
    s8 x1C;
    u8 pad1D;
    s16 x1E;
} Mover;
typedef struct { u8 pad[0x38]; Mover *x38; } Obj;
typedef struct { Vec3f p[4]; } Pts;
extern Pts D_800B8528;
extern f64 D_800B8558;

void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Normalize(Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
f32 Math_Atan2(f32, f32);

void PathMover_Update2(Obj *o) {
    Vec3f dir, pos, rel, target;
    Pts pts;
    f32 dot;
    Mover *m;

    pts = D_800B8528;
    m = o->x38;

    if (m->x1E > 0) {
        m->x1E--;
        return;
    }
    pos = m->pos;
    target = pts.p[m->x1C];
    Vec3f_Sub(&dir, &target, &pos);
    Vec3f_Normalize(&dir, &dir);
    Vec3f_Scale(&dir, &dir, 0.5f);
    Vec3f_Add(&pos, &pos, &dir);
    Vec3f_Sub(&rel, &pos, &m->pos);
    m->x10 = Math_Atan2(rel.x, rel.z) + D_800B8558;
    Vec3f_Sub(&rel, &pos, &target);
    Vec3f_Dot(&dot, &rel, &dir);
    if (dot > 0.0f) {
        if (++m->x1C == 2) {
            pos = pts.p[2];
            m->x1C = 3;
            m->x1E = 5;
        } else if (m->x1C >= 4) {
            pos = pts.p[0];
            m->x1C = 1;
            m->x1E = 5;
        } else {
            pos = target;
        }
    }
    m->pos = pos;
}
