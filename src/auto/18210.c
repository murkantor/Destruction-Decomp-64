#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { u8 pad[0x2C]; Gfx *x2C[2][2]; u8 pad3C[0x50]; s8 x8C; } Model;
typedef struct { s32 m[16]; } Mtx;
void guMtxL2F(Mtx *, s32);
Gfx *func_800112EC(Model *, Mtx *, Gfx *);

void func_80017610(Model *o, s32 *src, s32 k) {
    Mtx mtx;
    Gfx *g;
    s32 idx;

    if (k == 0) {
        o->x8C = (o->x8C + 1) % 2;
    }
    idx = o->x8C;
    guMtxL2F(&mtx, *src);
    g = func_800112EC(o, &mtx, o->x2C[k][idx]);
    g->w0 = 0xDF000000;
    g->w1 = 0;
}
