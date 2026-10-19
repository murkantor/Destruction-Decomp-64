f32 fabsf(f32);
f64 fabs(f64);
typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s16 x0; s16 x2; s16 pad4[2]; s16 x8; s16 xA; } Vp;
typedef struct {
    s16 x0;
    u16 x2;
    s16 x4;
    u16 x6;
    s16 x8;
    u16 xA;
    s16 xC;
    u16 xE;
    u8 pad10[0xC];
    u16 x1C;
    u16 x1E;
} Spr;
typedef struct { Vec3f pos; u8 padC[4]; Spr spr; } Sun;
typedef struct { u8 pad0[0xA8]; Sun *xA8; } World;
typedef struct { u8 pad0[0x1100]; World *x1100; } Game;
typedef struct { s32 mtx; u8 pad4[0x28]; u8 x2C[1]; } Cam;
typedef struct { f32 pos; f32 size; u8 r, g, b, a; } FlareDef;

extern Game *gGame;
extern u8 D_800D2E58;
extern f32 D_801312F8[];
extern FlareDef D_80084DD0[];
extern f64 D_800B86D0;
extern f64 D_800B86D8;
extern f64 D_800B86E0;
extern f64 D_800B86E8;
extern f64 D_800B86F0;
extern f64 D_800B86F8;
extern f64 D_800B8700;
extern f64 D_800B8708;
extern f64 D_800B8710;
void guMtxL2F(f32 mf[4][4], s32 m);
void func_80068FCC(f32 mf[4][4], f32, f32, f32, f32 *, f32 *, f32 *);
u8 Track_LineOfSight(void *, Vec3f *);
Gfx *Bg_Draw(Spr *, Gfx *, s32);

#define FTOU(x) ((x) > 0.0f ? (u32)(x) : 0)
#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Gfx_DrawLensFlare(Gfx *gfx, Cam *cam, Vp **vpp, s32 idx)
{
    Sun *sun;
    Vp *vp;
    Vec3f p;
    Vec3f out;
    f32 mf[4][4];
    f32 hw, hh, cx, cy;
    f32 w, nx, ny, dist, k, scale, alpha;
    f32 ahw, mnx, mny;
    u32 i;
    Spr *spr;
    volatile Spr *q;
    FlareDef *e;

    if (gGame->x1100 == NULL)
        return gfx;
    sun = gGame->x1100->xA8;
    if (sun == NULL)
        return gfx;
    if (D_800D2E58 >= 3)
        return gfx;

    vp = *vpp;
    hw = vp->x0 * 0.25f;
    hh = vp->x2 * 0.25f;
    cx = vp->x8 * 0.25f;
    cy = vp->xA * 0.25f;
    guMtxL2F(mf, cam->mtx);
    p.x = sun->pos.x;
    p.y = sun->pos.y;
    p.z = sun->pos.z;
    func_80068FCC(mf, p.x, p.y, p.z, &out.x, &out.y, &out.z);
    w = p.x * mf[0][3] + p.y * mf[1][3] + p.z * mf[2][3] + mf[3][3];
    ahw = fabsf(hw);
    if (w < 1.0f)
        goto off;
    nx = out.x / w;
    ny = out.y / w;
    dist = fabs(nx) + fabs(ny);
    if (dist >= D_800B86D0) {
    off:
        D_801312F8[idx] = 0.0f;
        return gfx;
    }
    if (Track_LineOfSight(cam->x2C, &p)) {
        D_801312F8[idx] = D_801312F8[idx] - D_800B86D8;
        if (D_801312F8[idx] <= 0.0) {
            D_801312F8[idx] = 0.0f;
            return gfx;
        }
    } else {
        D_801312F8[idx] = D_801312F8[idx] + D_800B86E0;
        if (D_801312F8[idx] > D_800B86E8)
            D_801312F8[idx] = 1.0f;
    }
    mnx = -nx;
    mny = -ny;
    if (dist <= 0.82f)
        k = 1.0f;
    else
        k = (D_800B86F0 - dist) * D_800B86F8;

    spr = &sun->spr;
    q = spr;
    scale = (D_800B8700 - dist) * D_800B8708 + D_800B8710;
    k *= D_801312F8[idx];
    scale *= D_801312F8[idx];
    GFX2(gfx, 0xE7000000, 0);
    GFX2(gfx, 0xE3000A01, 0);
    GFX2(gfx, 0xE3000C00, 0);
    GFX2(gfx, 0xE200001C, 0x504240);
    GFX2(gfx, 0xE3001201, 0x2000);
    GFX2(gfx, 0xFC119623, 0xFF2FFFFF);
    GFX2(gfx, 0xE3001001, 0);

    for (i = 0; i < 5; i++) {
        s32 isz, ix, iy;
        u32 a;
        s32 sx, sy;
        s32 x0, y0;
        e = &D_80084DD0[i];
        ix = (nx + e->pos * mnx) * hw + cx;
        iy = -(ny + e->pos * mny) * hh + cy;
        isz = e->size * ahw * scale;
        if (isz == 0)
            isz = 1;
        x0 = ix - isz + 1;
        y0 = iy - isz + 1;
        a = k * e->a;
        GFX2(gfx, 0xFA000000, (e->r << 24) | (e->g << 16) | (e->b << 8) | (a & 0xFF));
        q->x6 = isz * 8;
        sx = (q->x6 << 10) / q->x2;
        q->xE = isz * 8;
        sy = (q->xE << 10) / q->xA;
        q->x4 = x0 * 4;
        q->xC = y0 * 4;
        q->x1C = sx;
        q->x1E = sy;
        gfx = Bg_Draw(spr, gfx, 1);
    }
    if (dist < 0.8f) {
        u32 v = (0.8f - dist) * 150.0f;
        GFX2(gfx, 0xFA000000, v | 0xFFFFFF00);
        GFX2(gfx, 0xFCFFFFFF, 0xFFFDF6FB);
        {
            Gfx *_g = gfx++;
            u32 w0 = 0xF6000000 | ((FTOU(cx + cx) & 0x3FF) << 14) | ((FTOU(cy + cy) & 0x3FF) << 2);
            _g->w0 = w0;
            _g->w1 = 0;
        }
    }
    GFX2(gfx, 0xE7000000, 0);
    return gfx;
}
