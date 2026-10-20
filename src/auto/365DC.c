#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u16 v[4];
    Vec3f normal;
    u8 n;
    u8 pad15;
    s16 x16;
} Poly;

typedef struct {
    Vec3f p;
    f32 pad;
} Vert;

typedef struct {
    u16 start;
    u16 count;
} CellRange;

typedef struct {
    u16 w;
    u16 h;
    f32 scale;
    s16 ox;
    s16 oz;
    u16 cw;
    u16 ch;
    u8 pad40[0x10];
    u16 *cells;
    CellRange *ranges;
    u16 *polyList;
    Poly *polys;
    Vert *verts;
} Grid;

typedef struct {
    u8 pad0[0x30];
    Grid grid;
} Coll;

typedef struct {
    u8 pad0[0xC];
    Coll *coll;
} Track;

typedef struct {
    u8 pad0[0x70];
    Track *track;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *race;
} Game;

typedef struct {
    u8 pad0[0x290];
    Vec3f pos;
    u8 pad29C[0x904];
    s32 xBA0;
} Car;

typedef struct {
    s32 type;
    Vec3f normal;
    s32 n;
    Vec3f v[4];
} PolyInfo;

extern Game *gGame[];
s32 Surface_GetType(s32);

static inline s32 Grid_GetCell(Grid *c, Vec3f *pos) {
    s32 x = (pos->x * c->scale - c->ox) / c->cw;
    s32 z = (pos->z * c->scale - c->oz) / c->ch;

    if (x < 0 || x >= c->w || z < 0 || z >= c->h) {
        return -1;
    }
    return x * c->h + z;
}

static inline u8 PolyInfo_Contains(PolyInfo *info, Vec3f *pos) {
    Vec3f *prev = &info->v[info->n - 1];
    Vec3f *cur;
    s32 i;

    for (i = 0; i < info->n; i++) {
        cur = &info->v[i];
        if ((cur->z - prev->z) * (pos->x - prev->x) - (cur->x - prev->x) * (pos->z - prev->z) < 0.0) {
            return 0;
        }
        prev = cur;
    }
    return 1;
}

Poly *Car_GetFaceUnder(Car *car) {
    Poly *ret;
    Grid *g;
    Grid *c;
    Vec3f *pos;
    CellRange *r;
    Poly *poly;
    Vert *verts;
    PolyInfo info;
    PolyInfo *pi;
    Vert *vp;
    s32 cell;
    s32 i, j;
    s32 count;
    s32 start;
    s32 polyIdx;

    if (car->xBA0 == -1) {
        g = &gGame[0]->race->track->coll->grid;
        car->xBA0 = -2;
        ret = NULL;
        pos = &car->pos;
        cell = Grid_GetCell(&gGame[0]->race->track->coll->grid, pos);
        if (cell >= 0) {
            r = (CellRange *)((g->cells[cell] << 2) + (u32)g->ranges);
            count = r->count;
            start = r->start;
            for (i = 0; i < count; i++) {
                pi = &info;
                polyIdx = g->polyList[start + i];
                c = &gGame[0]->race->track->coll->grid;
                poly = &c->polys[polyIdx];
                verts = c->verts;
                info.n = poly->n;
                info.type = (s8)Surface_GetType(poly->x16);
                for (j = 0; j < pi->n; j++) {
                    vp = (Vert *)((poly->v[j] << 4) + (u32)verts);
                    pi->v[j].x = vp->p.x;
                    pi->v[j].y = vp->p.y;
                    pi->v[j].z = vp->p.z;
                }
                pi->normal = poly->normal;
                if (PolyInfo_Contains(&info, pos)) {
                    car->xBA0 = polyIdx;
                    ret = &g->polys[polyIdx];
                    break;
                }
            }
        }
    } else if (car->xBA0 == -2) {
        ret = NULL;
    } else {
        ret = &gGame[0]->race->track->coll->grid.polys[car->xBA0];
    }
    return ret;
}
