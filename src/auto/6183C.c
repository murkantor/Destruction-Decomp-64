#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; } Img60C3C;
typedef struct { s32 x0; s32 x4; s16 x8; s16 xA; s16 xC; s16 xE; s32 x10; s32 x14; } Tex60C3C;
typedef struct { u8 pad[0x1C]; Tex60C3C *x1C; } Obj60C3C;

void func_80060C3C(Obj60C3C *obj, Img60C3C *img) {
    if (img->x14 == 0x1033) {
        Tex60C3C *t = obj->x1C;
        t->x0 = img->x14;
        t->x4 = img->x0;
        t->x8 = 0;
        t->xA = ((img->xC * img->x10) >> (4 - img->x8)) - 1;
        t->xC = 0x7FF / (img->xC >> (4 - img->x8)) + 1;
        t->xE = 0;
        t->x10 = img->x0;
        t->x14 = -1;
    } else if (img->x14 == 0xFC1034) {
        Tex60C3C *t2 = obj->x1C;
        t2->x0 = img->x14;
        t2->x4 = img->x0;
        t2->x8 = 0;
        t2->xA = ((img->xC >> (4 - img->x8)) << 2) - 1;
        t2->xC = (img->x10 << 2) - 1;
        t2->xE = 0;
        t2->x10 = img->x0;
        t2->x14 = -1;
    }
}
