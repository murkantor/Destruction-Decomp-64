#include "common.h"
#include "m2c.h"

typedef struct { f32 x0, x4, x8, xC, x10, x14; } Box;
typedef struct { f32 x0, x4, x8, xC, x10, x14; f32 x18; u8 x1C; s16 x1E; } Shake;
typedef struct { u8 pad[0x38]; Shake *x38; } Obj;
void *Mem_Alloc(s32);

void Shake_Create(Obj *o, Box b) {
    Shake *p = Mem_Alloc(sizeof(Shake));

    p->x0 = b.x0;
    p->x4 = b.x4;
    p->x8 = b.x8;
    p->xC = b.xC;
    p->x10 = b.x10;
    p->x14 = b.x14;
    p->x18 = 2.0f;
    p->x1C = 1;
    p->x1E = 0;
    o->x38 = p;
}
