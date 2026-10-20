/*
 * The three functions at the start of the block Boot_LoadData copies from ROM
 * 0xBF790 to 0x80134280. The decompilation treats the block as data; resource
 * and object tables point at these, so the port has them in C
 * (tools/port/gen_data.py OVERLAY_FUNCS rewrites those pointers).
 */
#include "common.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

extern u8 D_80134280[];
Gfx *Gfx_SetupRenderMode(Gfx *gfx);
void *Mem_AllocAligned(u32 size, s32 alignMask, s32 alignOffset);
s32 Res_GetSize(s32 id);
void Res_Load(s32 id, void *dst);

/* 0x80134280 */
void *Ovl_ReturnArg(void *arg) {
    return arg;
}

/* 0x80134288: a draw handler that calls the display list at 0x801360D8 in the same block */
Gfx *Ovl_AppendDl(void *obj, Gfx *gfx) {
    Gfx *g = Gfx_SetupRenderMode(gfx);

    g->w0 = 0xDE000000; /* gSPDisplayList */
    g->w1 = (u32)(D_80134280 + (0x801360D8 - 0x80134280));
    return g + 1;
}

/* 0x801342C0: the same as Res_LoadAlloc */
void *Ovl_ResLoadAlloc(s32 id) {
    void *p = Mem_AllocAligned(Res_GetSize(id), 7, 0);

    Res_Load(id, p);
    return p;
}
