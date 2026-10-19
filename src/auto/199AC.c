#include "common.h"
#include "m2c.h"

typedef struct { u16 start; u16 count; } CellList;
typedef struct { u16 w; u16 h; } Dims;
typedef struct {
    u8 pad[0x30];
    Dims dims;
    u8 pad34[0x50 - 0x34];
    u16 *cells;
    CellList *lists;
    u16 *items;
} Grid;
typedef struct { u8 pad[0xC]; Grid *xC; } TrackX70;
typedef struct { u8 pad[0x70]; TrackX70 *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;

extern Game *gGame;
extern u8 D_801313A0[];

void func_80018DAC(s32 *out, s32 *count, s32 max, s32 x0, s32 z0, s32 x1, s32 z1) {
    Grid *g = gGame->track->x70->xC;
    Dims *d = &g->dims;
    u16 *items = g->items;
    u16 *cells = g->cells;
    CellList *lists = g->lists;
    s32 x, z, k, id;
    s32 bit;
    CellList *l;

    for (z = z0; z < z1 + 1; z++) {
        for (x = x0; x < x1 + 1; x++) {
            l = &lists[cells[x * d->h + z]];
            for (k = 0; k < l->count; k++) {
                id = items[l->start + k];
                D_801313A0[id / 8] |= 0x80 >> (id & 7);
            }
        }
    }
    *count = 0;
    for (z = z0; z < z1 + 1; z++) {
        for (x = x0; x < x1 + 1; x++) {
            l = &lists[cells[x * d->h + z]];
            for (k = 0; k < l->count; k++) {
                id = items[l->start + k];
                bit = 0x80 >> (id & 7);
                if (D_801313A0[id / 8] & bit) {
                    D_801313A0[id / 8] ^= bit;
                    out[*count] = id;
                    (*count)++;
                    if (*count >= max) {
                        return;
                    }
                }
            }
        }
    }
}
