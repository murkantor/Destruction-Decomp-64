#include "common.h"
#include "m2c.h"

typedef struct { u32 w[6]; } Blk18;
typedef struct { s32 x0; u8 pad[0x10]; Blk18 *x14; Blk18 *x18; Blk18 *x1C; Blk18 *x20; } Obj;
void func_80060810(Obj *);

Obj *Obj_Copy(Obj *dst, Obj *src) {
    func_80060810(src);
    func_80060810(dst);
    dst->x0 = src->x0;
    *dst->x14 = *src->x14;
    *dst->x18 = *src->x18;
    *dst->x1C = *src->x1C;
    *dst->x20 = *src->x20;
    return dst;
}
