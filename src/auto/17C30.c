#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

typedef struct {
    u8 pad[0x2C];
    u32 lists[11][2];
    u8 pad84[0x8];
    s32 cur;
} Model;

void func_80017030(Model *m, Gfx *g, s32 i) {
    s32 cur = m->cur;

    g->w0 = 0xDE000000;
    g->w1 = m->lists[i][cur];
}
