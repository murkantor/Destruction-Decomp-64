#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[4]; u8 w; u8 h; u8 pad6[0xE]; f32 ox; f32 oz; f32 cw; f32 ch; } Grid;
typedef struct { u8 pad[0xC]; Grid *grid; } Track;
typedef struct { u8 pad[0x70]; Track *track; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
extern Game *gGame;

s32 Track_GetGridCell(Vec3f pos) {
    s32 ix = (pos.x - gGame->race->track->grid->ox) / gGame->race->track->grid->cw;
    s32 iz = (pos.z - gGame->race->track->grid->oz) / gGame->race->track->grid->ch;

    if (ix < 0) {
        ix = 0;
    } else if (ix > gGame->race->track->grid->w - 1) {
        ix = gGame->race->track->grid->w - 1;
    }
    if (iz < 0) {
        iz = 0;
    } else if (iz > gGame->race->track->grid->h - 1) {
        iz = gGame->race->track->grid->h - 1;
    }
    return ix * gGame->race->track->grid->h + iz;
}
