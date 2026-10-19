#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

Gfx *func_8005793C(Gfx *g) {
    {
        Gfx *_g = g++;
        _g->w0 = 0xE9000000;
        _g->w1 = 0;
    }
    {
        Gfx *_g = g++;
        _g->w0 = 0xDF000000;
        _g->w1 = 0;
    }
    return g;
}
