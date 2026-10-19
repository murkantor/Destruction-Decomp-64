#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u16 idx[10];
    u8 n;
    u8 pad15[0x18 - 0x15];
} Poly;

typedef struct {
    Vec3f v;
    u8 padC[0x10 - 0xC];
} Vert;

typedef struct {
    u8 pad0[0x5C];
    Poly *polys;
    Vert *verts;
} Mesh;

typedef struct {
    u8 pad0[0xC];
    Mesh *xC;
} Track;

typedef struct {
    u8 pad0[0x70];
    Track *x70;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *x1100;
} Game;

extern Game *gGame;

void func_8001AB10(s32 *out, Vec3f *pos, s32 *list, s32 n) {
    f32 best = 100000.0f;
    Vert *a = NULL;
    Vert *b;
    s32 i;
    Poly *poly;
    u8 inside;
    s32 j;
    f32 dy;
    Vec3f e;
    Vec3f d;

    *out = -1;
    for (i = 0; i < n; i++) {
        poly = &gGame->x1100->x70->xC->polys[list[i]];
        inside = 1;
        for (j = 0; j < poly->n; j++) {
            a = &gGame->x1100->x70->xC->verts[poly->idx[j]];
            b = &gGame->x1100->x70->xC->verts[poly->idx[(j + 1) % poly->n]];
            e.x = b->v.x - a->v.x;
            e.y = 0.0f;
            e.z = b->v.z - a->v.z;
            d.x = pos->x - a->v.x;
            d.y = 0.0f;
            d.z = pos->z - a->v.z;
            if (e.z * d.x - e.x * d.z < 0.0f) {
                inside = 0;
            }
        }
        if (inside == 1) {
            dy = a->v.y - pos->y;
            if (dy < 0.0f) {
                dy = -dy;
            }
            if (dy < best) {
                best = dy;
                *out = list[i];
            }
            if (best < 5.0f) {
                return;
            }
        }
    }
}
