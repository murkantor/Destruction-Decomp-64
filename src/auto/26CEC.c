#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { Vec3f a; Vec3f b; Vec3f c; s32 x24; } Path;
typedef struct { u8 pad[0x38]; Path *x38; } Obj;
void *Mem_Alloc(s32);

void func_800260EC(Obj *o, Seg s) {
    Path *p = Mem_Alloc(sizeof(Path));

    p->a.x = s.a.x;
    p->a.y = s.a.y;
    p->a.z = s.a.z;
    p->b.x = s.b.x;
    p->b.y = s.b.y;
    p->b.z = s.b.z;
    p->c = p->b;
    p->x24 = 0;
    o->x38 = p;
}
