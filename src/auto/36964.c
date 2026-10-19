#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[2]; u16 x2; } GridEnt;
typedef struct {
    u16 w;
    u16 h;
    f32 scale;
    s16 ox;
    s16 oz;
    u16 sx;
    u16 sz;
    u8 pad10[0x20 - 0x10];
    u16 *cells;
    GridEnt *ents;
} GridInfo;
typedef struct { u8 pad[0x30]; GridInfo gi; } Grid;
typedef struct { u8 pad[0xC]; Grid *xC; } Unk70;
typedef struct { u8 pad[0x70]; Unk70 *x70; } Unk1100;
typedef struct { u8 pad[0x1100]; Unk1100 *x1100; } Game;
extern Game *gGame;

static inline s32 Grid_Index(GridInfo *gi, s32 x, s32 z) {
    if (x < 0 || x >= gi->w || z < 0 || z >= gi->h) {
        return -1;
    }
    return x * gi->h + z;
}

s32 Track_IsGridCellSolid(Vec3f *p) {
    Grid *g = gGame->x1100->x70->xC;
    s32 x = (p->x * g->gi.scale - g->gi.ox) / g->gi.sx;
    s32 z = (p->z * g->gi.scale - g->gi.oz) / g->gi.sz;
    GridInfo *gi = &g->gi;
    s32 idx = Grid_Index(gi, x, z);
    if (idx < 0) {
        return 0;
    }
    return gi->ents[gi->cells[idx]].x2 != 0;
}
