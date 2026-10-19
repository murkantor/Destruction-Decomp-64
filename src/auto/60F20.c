#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
extern u8 D_801301F9;
extern u8 D_B7560[];
extern u8 D_7BCC0[];

Gfx *Gfx_SetTextureLUT(Gfx *gfx) {
    if (D_801301F9) {
        Gfx *_g = gfx++;
        _g->w0 = 0xE1000000;
        _g->w1 = (u32)D_B7560;
        _g = gfx++;
        _g->w0 = 0xDD0007FF;
        _g->w1 = (u32)D_7BCC0;
    }
    return gfx;
}
