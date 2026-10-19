#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x30]; u16 w; u16 h; f32 scale; } TrackGrid;
typedef struct { u8 pad[0xC]; TrackGrid *grid; } TrackInfo;
typedef struct { u8 pad[0x70]; TrackInfo *info; } Level;
typedef struct { u8 pad[0x1100]; Level *level; } Game;
typedef struct { s32 x0; u8 face[0x44]; } FaceEntry;
extern Game *gGame;
void Track_PosToCell(s32 *cx, s32 *cz, f32 x, f32 z);
void Track_CollectCellItems(s32 *out, s32 *count, s32 max, s32 x0, s32 z0, s32 x1, s32 z1);
void Track_GetFace(void *o, s32 i);

s32 Track_GetFacesInRadius(Vec3f *p, f32 r, FaceEntry *out) {
    s32 buf[128];
    s32 x0, z0, x1, z1;
    s32 count;
    s32 i;
    TrackGrid *g = gGame->level->info->grid;
    f32 s, fx0, fx1, fz0, fz1;

    s = g->scale;
    fx0 = (p->x - r) * s;
    fx1 = (p->x + r) * s;
    fz0 = (p->z - r) * s;
    fz1 = (p->z + r) * s;
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
    Track_CollectCellItems(buf, &count, 0x80, x0, z0, x1, z1);
    for (i = 0; i < count; i++) {
        Track_GetFace(out[i].face, buf[i]);
    }
    return count;
}
