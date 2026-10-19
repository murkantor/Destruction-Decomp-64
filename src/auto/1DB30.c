#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f pos; u8 pad[0x18]; } Vtx24;
typedef struct { u8 pad[8]; s32 a; s32 b; s32 c; u8 pad2[4]; } Tri18;
extern u8 D_80091C7C;
extern Vtx24 D_80096320[];
extern Tri18 D_800960F0[];
extern Vec3f D_80130640[];
void Vec3f_Cross(Vec3f *out, Vec3f *a, Vec3f *b);
void Vec3f_Normalize(Vec3f *out, Vec3f *v);

void Init_TriNormals(void) {
    Vec3f e1;
    Vec3f e2;
    s32 i;

    if (D_80091C7C == 0) {
        D_80091C7C = 1;
        for (i = 0; i < 10; i++) {
            Tri18 *t = &D_800960F0[i];
            Vec3f *n = &D_80130640[i];
            Vec3f *a = &D_80096320[t->a].pos;
            Vec3f *b = &D_80096320[t->b].pos;
            Vec3f *c = &D_80096320[t->c].pos;
            f32 bx = b->x, by = b->y, bz = b->z;
            f32 x1 = bx - a->x;
            f32 y1 = by - a->y;
            f32 z1 = bz - a->z;
            f32 x2 = c->x - bx;
            f32 y2 = c->y - by;
            f32 z2 = c->z - bz;
            e1.x = x1; e1.y = y1; e1.z = z1;
            e2.x = x2; e2.y = y2; e2.z = z2;
            Vec3f_Cross(n, &e1, &e2);
            Vec3f_Normalize(n, n);
        }
    }
}
