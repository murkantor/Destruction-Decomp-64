#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { f32 x; f32 y; f32 z; u8 c[4]; } Vert;
typedef struct { u8 pad0[0x60]; Vert *x60; } Mesh;
typedef struct { u8 pad0[0xC]; Mesh *xC; } Track;
typedef struct { u8 pad0[0x70]; Track *x70; } Race;
typedef struct { u8 pad0[0x1100]; Race *race; } Game;
typedef struct { u16 i[4]; u8 pad8[0xC]; u8 x14; } Face;
extern Game *gGame;

#define VERTS (gGame->race->x70->xC->x60)

void Track_SampleFaceColor(f32 *out, f32 x, f32 z, Face *f) {
    Vert *v[3];
    Vec3f e1;
    Vec3f e2;
    f32 det;
    s32 i;

    if (f->x14 == 3) {
        v[0] = &VERTS[f->i[0]];
        v[1] = &VERTS[f->i[1]];
        v[2] = &VERTS[f->i[2]];
    } else {
        Vec3f a;
        Vec3f b;
        Vert *p0 = &VERTS[f->i[0]];
        Vert *p2 = &VERTS[f->i[2]];
        a.x = p2->x - p0->x;
        a.y = 0.0f;
        a.z = p2->z - p0->z;
        b.x = x - p0->x;
        b.y = 0.0f;
        b.z = z - p0->z;
        if (a.z * b.x - a.x * b.z < a.y) {
            v[0] = &VERTS[f->i[0]];
            v[1] = &VERTS[f->i[1]];
            v[2] = &VERTS[f->i[2]];
        } else {
            v[0] = &VERTS[f->i[0]];
            v[1] = &VERTS[f->i[2]];
            v[2] = &VERTS[f->i[3]];
        }
    }
    e1.x = v[1]->x - v[0]->x;
    e1.z = v[1]->z - v[0]->z;
    e2.x = v[2]->x - v[0]->x;
    e2.z = v[2]->z - v[0]->z;
    det = e2.x * e1.z - e2.z * e1.x;
    if (det == 0.0) {
        out[0] = v[0]->c[0];
        out[1] = v[0]->c[1];
        out[2] = v[0]->c[2];
        return;
    }
    for (i = 0; i < 3; i++) {
        u8 c0 = v[0]->c[i];
        f32 d2 = (f32)v[2]->c[i] - (f32)c0;
        f32 d1 = (f32)v[1]->c[i] - (f32)c0;
        f32 A = (d2 * e1.z - d1 * e2.z) / det;
        f32 t = d2 * e1.x;
        f32 B = (d1 * e2.x - t) / det;
        f32 C = (f32)v[0]->c[i] - A * v[0]->x - B * v[0]->z;
        out[i] = A * x + B * z + C;
        if (out[i] < 0.0f) {
            out[i] = 0.0f;
        }
        if (out[i] > 255.0f) {
            out[i] = 255.0f;
        }
    }
}
