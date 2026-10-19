#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

Gfx *func_80019020(Gfx *g) {
    {
        Gfx *_g = g++;
        _g->w0 = 0xE3001001;
        _g->w1 = 0;
    }
    {
        Gfx *_g = g++;
        _g->w0 = 0xD9C0F9FB;
        _g->w1 = 0;
    }
    return g;
}
