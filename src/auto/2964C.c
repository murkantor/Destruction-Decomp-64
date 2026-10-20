#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s16 x, y, z; u8 pad[0xA]; } DVtx;
typedef struct { u8 pad[0x14]; u16 n; u8 pad16[0xE]; DVtx *verts; } DMesh;
typedef struct { u8 pad[0x8]; } HeapBlock;
typedef struct {
    u8 pad0[0x190];
    DMesh *mesh;
    u8 pad194[0xB54 - 0x194];
    HeapBlock xB54;
    HeapBlock xB5C;
    HeapBlock xB64;
} DCar;

extern s32 D_800958B4;
void Car_WorldToLocal(DCar *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Length(f32 *, Vec3f *);
void *HeapBlock_Ptr(HeapBlock *);

static inline void deform(DVtx *v, s32 n, s32 *box, Vec3f *cp, f32 s) {
    Vec3f c = *cp;
    Vec3f unused;
    Vec3f t;
    Vec3f d;
    f32 len;
    f32 f;
    s32 i;

    for (i = 0; i < n; v++, i++) {
        if (v->x > box[1] || v->x < box[0] || v->z > box[3] || v->z < box[2]) {
            continue;
        }
        t.x = v->x;
        t.y = v->y;
        t.z = v->z;
        Vec3f_Sub(&d, &c, &t);
        Vec3f_Length(&len, &d);
        len *= 0.0011111111111111111;
        f = s - len;
        v->x = v->x * f;
        v->z = v->z * f;
    }
}

void Car_DeformMeshAtImpact(DCar *car, s32 force, Vec3f pos, s32 type) {
    s32 box[4];
    Vec3f p;
    f32 s;
    DMesh *mesh;
    DVtx *vtx;

    if (force < 2) {
        return;
    }
    Car_WorldToLocal(car, &pos, &p);
    Vec3f_Scale(&p, &p, 100.0f);
    box[0] = p.x - 50.0f;
    box[1] = p.x + 50.0f;
    box[2] = p.z - 50.0f;
    box[3] = p.z + 50.0f;
    if (type == 5 || type == 2) {
        force += 10;
    }
    s = 0.95 - force * 0.0015;
    mesh = car->mesh;
    vtx = mesh->verts;
    deform(vtx, mesh->n, box, &p, s);
    vtx = HeapBlock_Ptr(&car->xB54);
    deform(vtx, 8, box, &p, s);
    if (D_800958B4 == 2 || D_800958B4 == 11) {
        vtx = HeapBlock_Ptr(&car->xB5C);
        deform(vtx, 8, box, &p, s);
        vtx = HeapBlock_Ptr(&car->xB64);
        deform(vtx, 8, box, &p, s);
    }
}
