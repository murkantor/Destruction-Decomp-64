typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { Gfx *x0; void *x4; u8 pad8[0x10]; Gfx *x18; void *x1C; u8 pad20[0x108 - 0x20]; } ShBuf;
typedef struct {
    s32 x0;
    ShBuf buf[2];
    Vec3f pos;
    Vec3f c[4];
    Vec3f n[4];
    f32 d[4];
    Vec3f u;
    Vec3f v;
    Vec3f o;
    Vec3f nrm;
    f32 dist;
} Shadow;
typedef struct { Vec3f x; f32 pad0C; Vec3f y; f32 pad1C; Vec3f z; f32 pad2C; Vec3f pos; } Mtx3;
typedef struct { u8 pad[0x30]; u16 x30; u16 x32; } Grid;
typedef struct { u8 pad[0xC]; Grid *xC; } Cells;
typedef struct { u8 pad[0x70]; Cells *x70; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct { u8 pad[0xC]; f32 xC; } Scale;
extern Game *gGame;
extern Vec3f D_80084DC0;
extern const f64 D_800B86B8;
extern const f64 D_800B86C0;
extern Scale *D_800BC9A0;
extern s32 D_800BC9AC;
extern Gfx *D_800BC9B0;
void Vec3f_Sub(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_Dot(f32 *, Vec3f *, Vec3f *);
void Vec3f_Cross(Vec3f *, Vec3f *, Vec3f *);
void Vec3f_SetLength(Vec3f *, Vec3f *, f32);
void Points_BoundsXZ(f32 *, f32 *, f32 *, f32 *, Vec3f *, s32);
void Track_PosToCell(s32 *, s32 *, f32, f32);
void Track_CollectCellItems(s32 *, s32 *, s32, s32, s32, s32, s32);
Gfx *func_80019050(Gfx *);
void func_80014FF4(Shadow *, s32 *, s32);

#define GFX2(g, a, b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

void func_800158EC(Shadow *s, Mtx3 *m) {
    s32 items[200];
    Vec3f ab[2];
    Vec3f unused;
    Vec3f fwd;
    Vec3f side;
    f32 minX;
    f32 maxX;
    f32 minZ;
    f32 maxZ;
    s32 cx0;
    s32 cz0;
    s32 cx1;
    s32 cz1;
    s32 count;
    s32 i;
    ShBuf *buf;

    s->x0 = (s->x0 + 1) % 2;
    s->pos.x = m->pos.x;
    s->pos.y = m->pos.y;
    s->pos.z = m->pos.z;
    fwd.x = m->z.x * D_800B86B8;
    fwd.y = m->z.y * D_800B86B8;
    fwd.z = m->z.z * D_800B86B8;
    side.x = m->x.x * D_800B86C0;
    side.y = m->x.y * D_800B86C0;
    side.z = m->x.z * D_800B86C0;
    s->c[0].x = s->pos.x - fwd.x - side.x;
    s->c[0].y = s->pos.y - fwd.y - side.y;
    s->c[0].z = s->pos.z - fwd.z - side.z;
    s->c[1].x = s->pos.x + fwd.x - side.x;
    s->c[1].y = s->pos.y + fwd.y - side.y;
    s->c[1].z = s->pos.z + fwd.z - side.z;
    s->c[2].x = s->pos.x + fwd.x + side.x;
    s->c[2].y = s->pos.y + fwd.y + side.y;
    s->c[2].z = s->pos.z + fwd.z + side.z;
    s->c[3].x = s->pos.x - fwd.x + side.x;
    s->c[3].y = s->pos.y - fwd.y + side.y;
    s->c[3].z = s->pos.z - fwd.z + side.z;
    for (i = 0; i < 4; i++) {
        s32 j = (i + 1) % 4;
        Vec3f_Sub(&ab[0], &s->c[j], &s->c[i]);
        Vec3f_Sub(&ab[1], &D_80084DC0, &s->c[j]);
        Vec3f_Cross(&s->n[i], &ab[1], &ab[0]);
        Vec3f_SetLength(&s->n[i], &s->n[i], 1.0f);
        Vec3f_Dot(&s->d[i], &D_80084DC0, &s->n[i]);
    }
    Vec3f_Sub(&ab[0], &s->c[1], &s->c[0]);
    Vec3f_Sub(&ab[1], &s->c[3], &s->c[0]);
    s->o = s->c[0];
    Vec3f_SetLength(&s->u, &ab[0], 1.0f);
    Vec3f_SetLength(&s->v, &ab[1], 1.0f);
    Vec3f_Cross(&s->nrm, &s->u, &s->v);
    if (s->nrm.y > 0.0f) {
        s->nrm.x = -s->nrm.x;
        s->nrm.y = -s->nrm.y;
        s->nrm.z = -s->nrm.z;
    }
    Vec3f_Dot(&s->dist, &s->nrm, &s->o);
    Points_BoundsXZ(&minX, &maxX, &minZ, &maxZ, s->c, 4);
    minX *= D_800BC9A0->xC;
    minZ *= D_800BC9A0->xC;
    maxX *= D_800BC9A0->xC;
    maxZ *= D_800BC9A0->xC;
    Track_PosToCell(&cx0, &cz0, minX, minZ);
    Track_PosToCell(&cx1, &cz1, maxX, maxZ);
    if (cx0 < 0) {
        cx0 = 0;
    }
    if (cz0 < 0) {
        cz0 = 0;
    }
    if (cx1 >= gGame->x1100->x70->xC->x30) {
        cx1 = gGame->x1100->x70->xC->x30 - 1;
    }
    if (cz1 >= gGame->x1100->x70->xC->x32) {
        cz1 = gGame->x1100->x70->xC->x32 - 1;
    }
    if (cx0 <= cx1 && cz0 <= cz1) {
        Track_CollectCellItems(items, &count, 200, cx0, cz0, cx1, cz1);
    } else {
        count = 0;
    }
    buf = &s->buf[s->x0];
    buf->x18 = buf->x0;
    buf->x1C = buf->x4;
    D_800BC9B0 = NULL;
    D_800BC9AC = 0;
    buf->x18 = func_80019050(buf->x18);
    GFX2(buf->x18, 0xD9FFF9FF, 0);
    GFX2(buf->x18, 0xD9FFFFFF, 0x200005);
    GFX2(buf->x18, 0xFC121824, 0xFF33FFFF);
    GFX2(buf->x18, 0xE200001C, 0x5049D8);
    func_80014FF4(s, items, count);
    if (D_800BC9B0) {
        D_800BC9B0->w0 = 0x01000000 | ((D_800BC9AC & 0xFF) << 12) | ((D_800BC9AC & 0x7F) << 1);
        D_800BC9B0->w1 = (u32)buf->x1C;
    }
    GFX2(buf->x18, 0xDF000000, 0);
}
