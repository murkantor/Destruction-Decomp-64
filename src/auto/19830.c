#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x38];
    s16 originX;
    s16 originZ;
    u16 cellW;
    u16 cellH;
} Grid;
typedef struct {
    u8 pad[0xC];
    Grid *grid;
} Track;
typedef struct {
    u8 pad[0x70];
    Track *track;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
extern GameState *gGame;

void Track_PosToCell(s32 *cx, s32 *cz, f32 x, f32 z) {
    Grid *g = gGame->race->track->grid;
    f32 fx = (x - g->originX) / g->cellW;
    f32 fz = (z - g->originZ) / g->cellH;

    *cx = fx;
    *cz = fz;
}

typedef struct {
    f32 x, y, z;
} Vec3f;

void Points_BoundsXZ(f32 *minX, f32 *maxX, f32 *minZ, f32 *maxZ, Vec3f *pts, s32 n) {

    *minX = pts[0].x;
    *maxX = pts[0].x;
    *minZ = pts[0].z;
    *maxZ = pts[0].z;
    for (n--, pts++; n > 0; n--, pts++) {
        if (*minX > pts->x) {
            *minX = pts->x;
        }
        if (*maxX < pts->x) {
            *maxX = pts->x;
        }
        if (*minZ > pts->z) {
            *minZ = pts->z;
        }
        if (*maxZ < pts->z) {
            *maxZ = pts->z;
        }
    }
}
