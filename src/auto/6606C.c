#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x30];
    u8 *x30;
    u8 *x34;
    u8 pad38[0x40];
    u8 *x78;
    u8 pad7C[4];
    u8 *x80;
    u8 *x84;
    u8 pad88[0x12];
    s16 x9A;
    s16 x9C;
} Voice;

u8 *Seq_SetLoopPoints(Voice *v, u8 *p) {
    s32 ret;
    s32 off;

    ret = *p++ << 8;
    ret += *p++;
    off = *p++ << 8;
    off += *p++;
    v->x9A = 1;
    v->x34 = v->x84 + off;
    off = p[0] << 8;
    off += p[1];
    v->x9C = 1;
    v->x30 = v->x80 + off;
    return v->x78 + ret;
}
