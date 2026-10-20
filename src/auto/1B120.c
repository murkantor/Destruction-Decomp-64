#include "common.h"
#include "m2c.h"

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
    register s32 j ASM_REG("$7");
    s32 k;
    s32 y;
    s32 sum;
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
        sum = g->idx[k]; Grid_GetCell(g, &verts[sum], &cx, &cz);
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
        sum = g->idx[k]; Grid_GetCell(g, &verts[sum], &cx, &cz);
        ci = cz * g->nx + cx;
        while (k < g->cells[ci].start || k >= g->cells[ci].start + g->cells[ci].count) {
            p = (u16 *)((g->cells[ci].start + g->cells[ci].count++) * 2 + (u32)g->idx);
            t = *p;
            *p = g->idx[k];
            g->idx[k] = t;
            sum = g->idx[k]; Grid_GetCell(g, &verts[sum], &cx, &cz);
            ci = cz * g->nx + cx;
        }
    }
}
/* agent68: MATCH. The idx in all three Grid_GetCell calls goes through the same variable as the loop-3 running
 * total (sum = g->idx[k]; Grid_GetCell(g, &verts[sum], ...)), with sum a plain s32 (no asm("$4")). That makes it one
 * global pseudo that global-alloc puts in a0, which breaks the lhu->sll tie. j asm("$7") is still needed. */
