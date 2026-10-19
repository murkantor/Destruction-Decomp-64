#include "common.h"
#include "m2c.h"

typedef struct { f32 x0, x4, x8, xC, x10, x14; } Box;
typedef struct { u8 pad[0x38]; Box *x38; } Obj;
void *Mem_Alloc(s32);

void func_80025FCC(Obj *o, Box b) {
    Box *p = Mem_Alloc(sizeof(Box));

    p->x0 = b.x0;
    p->x4 = b.x4;
    p->x8 = b.x8;
    p->xC = b.xC;
    p->x10 = b.x10;
    p->x14 = b.x14;
    o->x38 = p;
}
