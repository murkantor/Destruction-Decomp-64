#include "common.h"
#include "m2c.h"

typedef struct { s32 a, b, c; } Tri;
typedef struct { f32 x, y, z, w; } Vec4;
typedef struct { u16 idx[4]; Tri x8; u8 x14; u8 pad; s16 x16; } Face;
typedef struct { u8 pad[0x5C]; Face *x5C; Vec4 *x60; } Mesh;
typedef struct { u8 pad[0xC]; Mesh *xC; } Trk;
typedef struct { u8 pad[0x70]; Trk *x70; } Sub1100;
typedef struct { u8 pad[0x1100]; Sub1100 *x1100; } Game;
extern Game *gGame;
extern s8 Surface_GetType(s32);
typedef struct { f32 x, y, z; } Vec3;
typedef struct { s32 x0; Tri x4; s32 x10; Vec3 v[4]; } Out;
void Track_GetFace(Out *o, s32 i) {
    Mesh *m = gGame->x1100->x70->xC;
    Face *f = &m->x5C[i];
    Vec4 *verts = m->x60;
    s32 j;
    o->x10 = f->x14;
    o->x0 = Surface_GetType(f->x16);
    for (j = 0; j < o->x10; j++) {
        Vec4 *v = (Vec4 *)(f->idx[j] * 16 + (s32)verts);
        o->v[j].x = v->x;
        o->v[j].y = v->y;
        o->v[j].z = v->z;
    }
    PORT_CONVERT(&f->x8, 3, 4); /* o is usually a stack array */
    o->x4 = f->x8;
}
