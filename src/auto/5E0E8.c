#include "common.h"
#include "m2c.h"

typedef struct {
    s32 w[3];
} Block3;
typedef struct {
    u16 x0;
    u16 x2;
    u8 pad4[8];
    s32 xC;
    u16 x10;
    u8 pad12[2];
    s32 x14;
    Block3 b1;
    Block3 b2;
    f32 x30;
    f32 x34;
    u8 pad38[8];
    f32 x40;
    s32 x44;
    s32 x48;
    s32 x4C;
    u8 x50;
} Template;
typedef struct {
    u16 x0;
    u16 x2;
    u8 pad4[8];
    s32 xC;
    u16 x10;
    u8 pad12[2];
    s32 x14;
    Block3 b1;
    Block3 b2;
    f32 x30;
    f32 x34;
    f32 x38;
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
    u8 x4C;
} Item;

void Snd_InitFromTemplate(Item *dst, Template *src) {
    dst->x0 = src->x0;
    dst->x2 = src->x2;
    dst->xC = src->xC;
    dst->x10 = src->x10;
    dst->x14 = src->x14;
    dst->b1 = src->b1;
    dst->b2 = src->b2;
    dst->x30 = src->x30;
    dst->x34 = src->x34;
    dst->x38 = src->x40;
    dst->x44 = src->x48;
    dst->x48 = src->x4C;
    dst->x4C = src->x50;
    dst->x3C = 0;
    dst->x40 = 2;
}
