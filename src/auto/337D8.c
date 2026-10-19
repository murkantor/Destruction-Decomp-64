#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f v; u8 pad[0x18]; } SrcVtx;

typedef struct {
    u8 pad0[0xA4];
    Vec3f trans;
    u8 padB0[0x18];
    Vec3f r0;
    Vec3f r1;
    Vec3f r2;
    u8 padEC[0x140 - 0xEC];
    Vec3f *x140;
    Vec3f *x144;
    s32 x148;
    u8 pad14C[0xC];
    SrcVtx *x158;
    Vec3f *x15C;
    s32 x160;
} Model;

void func_80032BD8(Model *m) {
    s32 i;
    Vec3f *t = &m->trans;
    Vec3f *r0 = &m->r0;
    Vec3f *r1 = &m->r1;
    Vec3f *r2 = &m->r2;

    for (i = 0; i < m->x160; i++) {
        Vec3f *s = &m->x158[i].v;
        Vec3f *d;
        f32 x = s->x;
        f32 y = s->y;
        f32 z = s->z;
        d = &m->x15C[i];
        d->x = r0->x * x + r1->x * y + r2->x * z + t->x;
        d->y = r0->y * x + r1->y * y + r2->y * z + t->y;
        d->z = r0->z * x + r1->z * y + r2->z * z + t->z;
    }
    for (i = 0; i < m->x148; i++) {
        Vec3f *s = &m->x140[i];
        Vec3f *d = &m->x144[i];
        d->x = r0->x * s->x + r1->x * s->y + r2->x * s->z;
        d->y = r0->y * s->x + r1->y * s->y + r2->y * s->z;
        d->z = r0->z * s->x + r1->z * s->y + r2->z * s->z;
    }
}
