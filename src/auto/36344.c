#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x30]; u16 w; u16 h; f32 scale; s16 ox; s16 oy; u16 cw; u16 ch; } Grid;
typedef struct { u8 pad[0xC]; Grid *grid; } Track;
typedef struct { u8 pad[0x70]; Track *track; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
extern Game *gGame;

s32 Track_GetCellIndex(f32 x, f32 y) {
    Grid *g = gGame->race->track->grid;
    s32 ix = (x * g->scale - g->ox) / g->cw;
    s32 iy = (y * g->scale - g->oy) / g->ch;
    if (ix < 0 || ix >= g->w || iy < 0 || iy >= g->h) {
        return -1;
    }
    return ix * g->h + iy;
}
