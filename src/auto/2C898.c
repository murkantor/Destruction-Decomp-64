#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[4];
    u8 nx;
    u8 nz;
    u8 pad6[0x14 - 6];
    f32 ox;
    f32 oz;
    f32 sx;
    f32 sz;
} GridHdr;
typedef struct { u8 pad[0xC]; GridHdr *grid; } Track70;
typedef struct { u8 pad[0x44]; s32 count; void *objs; u8 pad4C[0x70 - 0x4C]; Track70 *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *track; } Game;
typedef struct {
    u8 pad0[4];
    s32 active;
    f32 x8;
    f32 xC;
    f32 x10;
    f32 x14;
    f32 x18;
    f32 x1C;
    u8 pad20[8];
} GridObj;

extern Game *gGame;
extern u32 Res_GetSize(s32 id);
extern void *Mem_AllocAligned(u32 size, s32 align, s32 flags);
extern void Res_Load(s32 id, void *dst);
extern void Grid_AddObjToCellsInRect(GridObj *obj, f32 x0, f32 z0, f32 x1, f32 z1);

void *Track_LoadGridObjects(s32 id)
{
    GridHdr *grid = gGame->track->x70->grid;
    u32 size;
    GridObj *objs;
    Track *t;
    GridObj *o;
    s32 i;
    f32 minX, minZ, maxX, maxZ;
    f32 x0, z0, x1, z1;
    f32 minX2, maxX2, minZ2, maxZ2;

    size = Res_GetSize(id);
    objs = Mem_AllocAligned(size, 0x3F, 0);
    t = gGame->track;
    t->objs = objs;
    t->count = size / 40;
    Res_Load(id, objs);
    minX = grid->ox + 1.0f;
    minZ = grid->oz + 1.0f;
    maxX = (grid->nx * grid->sx + grid->ox) - 1.0f;
    maxZ = (grid->nz * grid->sz + grid->oz) - 1.0f;
    for (i = 0; i < t->count; i++) {
        o = &objs[i];
        if (o->active) {
            minX2 = o->x8; if (!(minX2 <= o->x14)) minX2 = o->x14;
            x0 = minX2 - 10.0f;
            maxX2 = o->x8; if (!(o->x14 <= maxX2)) maxX2 = o->x14;
            x1 = maxX2 + 10.0f;
            minZ2 = o->x10; if (!(minZ2 <= o->x1C)) minZ2 = o->x1C;
            z0 = minZ2 - 10.0f;
            maxZ2 = o->x10; if (!(o->x1C <= maxZ2)) maxZ2 = o->x1C;
            z1 = maxZ2 + 10.0f;
            if (x0 < minX) x0 = minX;
            if (z0 < minZ) z0 = minZ;
            if (maxX < x1) x1 = maxX;
            if (maxZ < z1) z1 = maxZ;
            Grid_AddObjToCellsInRect(o, x0, z0, x1, z1);
        }
    }
    return objs;
}
