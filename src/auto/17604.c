#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;
typedef struct {
    s32 m[4][4];
} Mtx;
typedef struct {
    u8 pad[0x2C];
    Gfx *dl[8][2];
    u8 pad6C[0x20];
    s32 frame;
} Model;
void guMtxL2F(Mtx *, s32);
Gfx *func_800112EC(Model *, Mtx *, Gfx *);

void func_80016A04(Model *m, s32 *arg, s32 layer) {
    Mtx mtx;
    Gfx *g;
    s32 buf;

    if (layer == 0) {
        m->frame = (m->frame + 1) % 2;
    }
    buf = m->frame;
    guMtxL2F(&mtx, *arg);
    g = func_800112EC(m, &mtx, m->dl[layer][buf]);
    g->w0 = 0xDF000000;
    g->w1 = 0;
}
