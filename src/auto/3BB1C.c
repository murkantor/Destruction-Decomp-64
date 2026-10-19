#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 w; u8 h; u8 pad2[0x26]; void **cells; } GridDim;
typedef struct { u8 pad[4]; GridDim d; } Grid;
typedef struct { u8 pad[0xC]; Grid *grid; } Track;
typedef struct { u8 pad[0x70]; Track *track; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
extern Game *gGame;
s32 func_80011CCC(Vec3f pos);
void func_80063FA0(void **, void *);

void func_8003AF1C(void *obj, f32 x0, f32 z0, f32 x1, f32 z1) {
    Vec3f v;
    s32 idx[4];
    Grid *grid = gGame->race->track->grid;
    s32 n, i, j, lo, hi;
    GridDim *d;

    v.x = x0;
    v.z = z0;
    idx[0] = func_80011CCC(v);
    v.z = z1;
    idx[1] = func_80011CCC(v);
    v.x = x1;
    idx[3] = func_80011CCC(v);
    v.z = z0;
    idx[2] = func_80011CCC(v);
    n = (idx[2] - idx[0]) / grid->d.w;
    d = &grid->d;
    lo = idx[0];
    hi = idx[1];
    for (i = 0; i <= n; i++) {
        for (j = lo; j <= hi; j++) {
            func_80063FA0(&d->cells[j], obj);
        }
        lo += d->w;
        hi += d->w;
    }
}
