#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 m[3][3]; } Mtx3f;

Vec3f Vec3f_TransformTransposed(Mtx3f *m, Vec3f *t, Vec3f *v) {
    Vec3f a;
    Vec3f r;
    r.x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
    r.y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
    r.z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
    a = r;
    a.x += t->x;
    a.y += t->y;
    a.z += t->z;
    return a;
}
