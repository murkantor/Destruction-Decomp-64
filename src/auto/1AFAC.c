#include "common.h"
#include "m2c.h"

typedef struct { s16 x; s16 y; s16 z; s16 pad[5]; } Vtx1A3AC;
typedef struct { u8 pad0[8]; s32 x8; s32 xC; s32 x10; s32 x14; u16 *x18; u8 pad1C[8]; s32 x24; } Obj1A3AC;

void Mesh_ComputeBoundsXZ(Obj1A3AC *o, Vtx1A3AC *verts) {
    s32 i;
    Vtx1A3AC *v;
    Vtx1A3AC *f;
    char unused[1];

    f = (Vtx1A3AC *)(o->x18[0] * 16 + (u32)verts);
    o->x10 = o->x8 = f->x;
    o->x14 = o->xC = f->z;
    for (i = 0; i < o->x24; i++) {
        v = (Vtx1A3AC *)(o->x18[i] * 16 + (u32)verts);
        if (v->x < o->x8) {
            o->x8 = v->x;
        }
        if (v->x > o->x10) {
            o->x10 = v->x;
        }
        if (v->z < o->xC) {
            o->xC = v->z;
        }
        if (v->z > o->x14) {
            o->x14 = v->z;
        }
    }
    o->x10++;
    o->x14++;
}
