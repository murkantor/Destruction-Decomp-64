#include "common.h"
#include "m2c.h"

typedef struct {
    s16 x0;
    s16 x2;
    s16 x4;
    s16 x6;
    s16 x8;
    s16 xA;
    s16 xC;
    s16 xE;
    s16 x10;
    s16 x12;
    u8 x14;
    u8 x15;
    u8 x16;
    u8 x17;
} S1;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s16 x10;
    s16 x12;
    s16 x14;
    s16 x16;
} S2;

typedef struct {
    s32 x0;
    s32 x4;
    s16 x8;
    s16 xA;
    s16 xC;
    s16 xE;
    s32 x10;
    s32 x14;
} S3;

typedef struct {
    s32 x0;
    s32 x4;
    s16 x8;
    s16 xA;
    s16 xC;
    s16 xE;
    s32 x10;
    s32 x14;
} S4;

typedef struct {
    s32 x0;
    u8 pad4[0x10];
    S1 *x14;
    S2 *x18;
    S3 *x1C;
    S4 *x20;
} Obj;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
} Cfg;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
} Opt;

void func_80060DA8(Obj *o, s32 arg1, Cfg *c, Opt *opt) {
    S1 *s1;
    S2 *s2;
    S3 *s3;
    S4 *s4;

    s1 = o->x14;
    o->x0 = arg1;
    s1->x0 = 0;
    s1->x8 = 0;
    s1->x2 = 0x400;
    s1->xA = 0x400;
    s1->x6 = 0;
    s1->xE = 0;
    s1->x10 = c->xC >> (4 - c->x8);
    s1->x12 = 0;
    s1->x14 = c->x4;
    s1->x15 = c->x8;
    s1->x4 = c->xC << 5;
    s1->xC = c->x10 << 5;
    s1->x16 = 0;
    s1->x17 = 0;

    s2 = o->x18;
    s2->x0 = 0x10000;
    s2->x4 = 0;
    s2->x8 = 0;
    s2->xC = 0x10000;
    s2->x12 = 0;
    s2->x10 = 0;
    s2->x16 = 0x400;
    s2->x14 = 0x400;

    if (c->x14 == 0x1033) {
        s3 = o->x1C;
        s3->x0 = c->x14;
        s3->x4 = c->x0;
        s3->x8 = 0;
        s3->xA = ((c->xC * c->x10) >> (4 - c->x8)) - 1;
        s3->xC = 0x7FF / (c->xC >> (4 - c->x8)) + 1;
        s3->xE = 0;
        s3->x10 = c->x0;
        s3->x14 = -1;
    } else if (c->x14 == 0xFC1034) {
        s3 = o->x1C;
        s3->x0 = c->x14;
        s3->x4 = c->x0;
        s3->x8 = 0;
        s3->xA = ((c->xC >> (4 - c->x8)) << 2) - 1;
        s3->xC = (c->x10 << 2) - 1;
        s3->xE = 0;
        s3->x10 = c->x0;
        s3->x14 = -1;
    }

    s4 = o->x20;
    if (opt != NULL) {
        s4->x0 = 0x30;
        s4->x4 = opt->x0;
        s4->x8 = opt->x4;
        s4->xA = opt->x8;
        s4->xC = 0;
        s4->xE = 0;
        s4->x10 = -1;
        s4->x14 = 0;
    } else {
        s4->x0 = 0;
        s4->x4 = 0;
    }
}
