#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x30]; u16 w; u16 h; f32 scale; } TrackGrid;
typedef struct { u8 pad[0xC]; TrackGrid *grid; } TrackInfo;
typedef struct { u8 pad[0x70]; TrackInfo *info; } Level;
typedef struct { u8 pad[0x1100]; Level *level; } Game;
extern Game *gGame;
void Track_PosToCell(s32 *cx, s32 *cz, f32 x, f32 z);
void Track_CollectCellItems(s32 *out, s32 *count, s32 max, s32 x0, s32 z0, s32 x1, s32 z1);

s32 Track_GetCellsInBox(Vec3f *a, Vec3f *b, s32 *out, s32 max) {
    s32 x0, z0, x1, z1;
    s32 count;
    TrackGrid *g = gGame->level->info->grid;
    f32 s;
    f32 fx0, fx1, fz0, fz1;
    f32 t0, t1, t2, t3;

    t0 = a->x; if (!(t0 <= b->x)) t0 = b->x;
    fx0 = t0 * (s = g->scale);
    t1 = a->x; if (!(b->x <= t1)) t1 = b->x;
    fx1 = t1 * s;
    t2 = a->z; if (!(t2 <= b->z)) t2 = b->z;
    fz0 = t2 * s;
    t3 = a->z; if (!(b->z <= t3)) t3 = b->z;
    fz1 = t3 * s;
    Track_PosToCell(&x0, &z0, fx0, fz0);
    Track_PosToCell(&x1, &z1, fx1, fz1);
    if (x0 < 0) x0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 < 0) x1 = 0;
    if (z1 < 0) z1 = 0;
    if (x0 > gGame->level->info->grid->w - 1) x0 = gGame->level->info->grid->w - 1;
    if (z0 > gGame->level->info->grid->h - 1) z0 = gGame->level->info->grid->h - 1;
    if (x1 > gGame->level->info->grid->w - 1) x1 = gGame->level->info->grid->w - 1;
    if (z1 > gGame->level->info->grid->h - 1) z1 = gGame->level->info->grid->h - 1;
    Track_CollectCellItems(out, &count, max, x0, z0, x1, z1);
    return count;
}
