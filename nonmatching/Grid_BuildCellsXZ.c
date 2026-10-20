typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u8 pad[10];
} GVert;

typedef struct {
    s32 start;
    s32 count;
} GCell;

typedef struct {
    s32 nx;
    s32 nz;
    s32 minX;
    s32 minZ;
    s32 maxX;
    s32 maxZ;
    u16 *idx;
    u8 pad[8];
    s32 count;
    u8 pad2[4];
    GCell *cells;
} Grid;

typedef struct {
    s32 x;
    s32 z;
} CellPos;

void Mesh_ComputeBoundsXZ(Grid *, GVert *);

static inline void Grid_GetCell(Grid *g, GVert *v, s32 *cx, s32 *cz) {
    *cx = (v->x - g->minX) * g->nx / (g->maxX - g->minX);
    *cz = (v->z - g->minZ) * g->nz / (g->maxZ - g->minZ);
}

void Grid_BuildCellsXZ(Grid *g, GVert *verts) {
    s32 i;
    register s32 j asm("$7");
    s32 k;
    s32 y;
    register s32 sum asm("$4");
    s32 ci;
    s32 cx;
    s32 cz;
    u16 *p;
    u16 t;

    Mesh_ComputeBoundsXZ(g, verts);
    for (i = 0; i < g->nx; i++) {
        for (j = 0; j < g->nz; j++) {
            g->cells[j * g->nx + i].count = 0;
            g->cells[j * g->nx + i].start = -1;
        }
    }
    for (k = 0; k < g->count; k++) {
        Grid_GetCell(g, &verts[g->idx[k]], &cx, &cz);
        g->cells[cz * g->nx + cx].count++;
    }
    y = 0;
    sum = 0;
    if (g->nz > 0) {
        do {
            char unused[8];
        for (i = 0; i < g->nx; i++) {
            g->cells[y * g->nx + i].start = sum;
            sum += g->cells[y * g->nx + i].count;
            g->cells[y * g->nx + i].count = 0;
        }
            y++;
        } while (y < g->nz);
    }
    for (k = 0; k < g->count; k++) {
        Grid_GetCell(g, &verts[g->idx[k]], &cx, &cz);
        ci = cz * g->nx + cx;
        while (k < g->cells[ci].start || k >= g->cells[ci].start + g->cells[ci].count) {
            p = (u16 *)((g->cells[ci].start + g->cells[ci].count++) * 2 + (u32)g->idx);
            t = *p;
            *p = g->idx[k];
            g->idx[k] = t;
            Grid_GetCell(g, &verts[g->idx[k]], &cx, &cz);
            ci = cz * g->nx + cx;
        }
    }
}
/* agent63: 11 diff lines (was 25). Fixed loops 1 and 3 with hard-register locals j asm("$7") and sum asm("$4"),
 * loop 3 written as "if (g->nz > 0) do { ... y++; } while (y < g->nz);" with its own counter y (so combine does not
 * re-emit j=0 after sum=0; the blez delay slot then gets move a0,zero), plus "char unused[8]" inside the do body to
 * restore the 88-byte frame. Left: the three Grid_GetCell idx/vertex-pointer registers (local-alloc ties lhu->sll
 * in loop 2 and loop 4 first call; original keeps idx in a0 apart, and in loop 4's second call uses a0 for the chain
 * and v1 for minX). Hard-reg idx locals are copy-propagated away by CSE; shared idx variables, F2-F5 inline forms
 * (locals for x/z/minX, result temps, nx*(..)) and macros did not help. */
