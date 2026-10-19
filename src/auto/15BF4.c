#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 x, y, z; u16 flag; s16 s, t; s8 r, g, b; u8 a; } Vtx;
typedef struct { Vec3f pos; f32 u; f32 v; } ClipVtx;
typedef struct { u16 idx[10]; u8 n; u8 pad[3]; } Face;
typedef struct { f32 x, y, z, w; } MeshVtx;
typedef struct { u8 pad[0x5C]; Face *faces; MeshVtx *verts; } Mesh;
typedef struct {
    u8 pad0[0x18];
    Gfx *gfx;
    Vtx *vtx;
    u8 pad20[0x108 - 0x20];
} Seg;
typedef struct {
    s32 x0;
    Seg segs[2];
    u8 pad[0x214 - 0x214];
} CtxHead;
typedef struct {
    s32 x0;
    Seg segs[2];
    Vec3f x214;
    u8 pad220[0x250 - 0x220];
    u8 x250[0xC];
    u8 x25C[0xC];
    u8 x268[0xC];
    u8 x274[0xC];
    f32 x280;
    f32 x284;
    f32 x288;
    f32 x28C;
    Vec3f x290;
    Vec3f x29C;
    Vec3f x2A8;
    Vec3f x2B4;
    f32 x2C0;
} Ctx;

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

extern Vec3f D_80084DC0;
extern u8 D_80083DC0[];
extern Mesh *D_800BC9A4;
extern s32 D_800BC9AC;
extern Gfx *D_800BC9B0;

void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Add(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Scale(Vec3f *, Vec3f *, f32);
s32 Poly_ClipSH(ClipVtx *out, ClipVtx *in, s32 n, void *plane, f32 d);

s32 Shadow_ClipAndEmitFaces(Ctx *ctx, s32 *list, s32 count) {
    ClipVtx bufA[10];
    ClipVtx bufB[10];
    f32 d;
    Seg *seg;
    s32 i;
    s32 j;
    s32 n;
    s32 tris;
    u8 k;
    ClipVtx *vert;
    f32 s;

    seg = &ctx->segs[ctx->x0];
    Vec3f_Dot(&d, &ctx->x2B4, &D_80084DC0);
    d -= ctx->x2C0;
    GFX2(seg->gfx, 0xFD180000, D_80083DC0);
    GFX2(seg->gfx, 0xF5180000, 0x07097E5F);
    GFX2(seg->gfx, 0xE6000000, 0);
    GFX2(seg->gfx, 0xF3000000, 0x073FF080);
    GFX2(seg->gfx, 0xE7000000, 0);
    GFX2(seg->gfx, 0xF5181000, 0x00097E5F);
    GFX2(seg->gfx, 0xF2000000, 0x0007C07C);
    tris = 0;
    for (i = 0; i < count; i++) {
        Face *f = &D_800BC9A4->faces[list[i]];
        n = f->n;
        for (j = 0; j < n; j++) {
            MeshVtx *mv = &D_800BC9A4->verts[f->idx[j]];
            bufA[j].pos.x = mv->x;
            bufA[j].pos.y = mv->y;
            bufA[j].pos.z = mv->z;
        }
        if ((n = Poly_ClipSH(bufB, bufA, n, ctx->x250, ctx->x280)) == 0) continue;
        if ((n = Poly_ClipSH(bufA, bufB, n, ctx->x25C, ctx->x284)) == 0) continue;
        if ((n = Poly_ClipSH(bufB, bufA, n, ctx->x268, ctx->x288)) == 0) continue;
        if ((n = Poly_ClipSH(bufA, bufB, n, ctx->x274, ctx->x28C)) == 0) continue;
        if ((n = Poly_ClipSH(bufB, bufA, n, &ctx->x2B4, ctx->x2C0)) == 0) continue;
        vert = bufB;
        if (n < 3) continue;
        if (D_800BC9AC + n > 32) {
            D_800BC9B0->w0 = 0x01000000 | ((D_800BC9AC & 0xFF) << 12) | ((D_800BC9AC & 0x7F) << 1);
            D_800BC9B0->w1 = (u32)seg->vtx;
            seg->vtx += D_800BC9AC;
            D_800BC9AC = 0;
        }
        if (D_800BC9AC == 0) {
            D_800BC9B0 = seg->gfx;
            GFX2(seg->gfx, 0x01020040, seg->vtx);
        }
        for (k = 0; k < n; k++) {
            ClipVtx *v = &vert[k];
            Vtx *vtx;
            Vec3f w;
            f32 t;
            Vec3f_Dot(&t, &ctx->x2B4, &v->pos);
            t -= ctx->x2C0;
            s = d / (d - t);
            Vec3f_Sub(&w, &v->pos, &D_80084DC0);
            Vec3f_Scale(&w, &w, s);
            Vec3f_Add(&w, &w, &D_80084DC0);
            Vec3f_Sub(&w, &w, &ctx->x2A8);
            Vec3f_Dot(&v->u, &w, &ctx->x290);
            Vec3f_Dot(&v->v, &w, &ctx->x29C);
            v->u = v->u * 1024.0f * 0.3125;
            v->v = v->v * 1024.0f * 0.17857142857142858;
            vtx = &seg->vtx[D_800BC9AC];
            vtx->x = (v->pos.x - ctx->x214.x) * 128.0f;
            vtx->y = (v->pos.y - ctx->x214.y) * 128.0f;
            vtx->z = (v->pos.z - ctx->x214.z) * 128.0f;
            vtx->flag = 0;
            vtx->s = v->u;
            vtx->t = v->v;
            vtx->r = -1;
            vtx->g = -1;
            vtx->b = -1;
            vtx->a = 0x80;
            D_800BC9AC++;
        }
        D_800BC9AC -= n;
        for (j = 0; j < n - 2; j++) {
            tris++;
            GFX2(seg->gfx, ((((D_800BC9AC * 2) & 0xFF) << 16) | ((((D_800BC9AC + j + 1) * 2) & 0xFF) << 8) | (((D_800BC9AC + j + 2) * 2) & 0xFF)) | 0x05000000, 0);
        }
        D_800BC9AC += n;
    }
    return tris;
}
