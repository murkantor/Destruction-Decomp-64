#include "common.h"
#include "m2c.h"

typedef struct { s32 id; u8 tex[0x18]; } TexSlot;
typedef struct { u8 pad[0xC]; s32 xC; s32 x10; } TexInfo;
typedef struct {
    s32 x0; s32 x4; s32 x8; u8 xC; u8 padD[3];
    TexInfo *x10; void *x14;
    f32 x18, x1C, x20, x24, x28, x2C, x30;
    s32 x34, x38;
    f32 x3C, x40;
} SpriteEnt;
typedef struct {
    u8 pad0[0xC];
    SpriteEnt spr[441];
    u8 pad1[0x7588 - 0xC - 441 * 0x44];
    TexSlot tex[184];
    u8 pad2[0x8B5C - 0x7588 - 184 * 0x1C];
    s16 nTex;
    s16 nSpr;
} SpriteMgr;
typedef struct {
    s32 x0; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[3];
    s32 x14; s32 x18;
    f32 x1C, x20, x24, x28, x2C, x30, x34;
    u8 pad38[4];
    u8 x3C; u8 pad3D[3];
    s32 x40; s32 x44;
} SpriteDesc;
typedef struct { s32 x0, x4, x8, xC, x10, x14, x18; s32 pad[3]; } ResInfo;
extern void *gUi;
void Res_Load(s32, void *);
void *Res_Get(s32);
void Tex_Init(void *, void *, s32, s32, s32, s32, s32);
void *Sprite_Create(void);
void Sprite2D_InitObjs(void *, s32, void *, s32);
void Sprite_SetPrimDepth(void *, s32, s32);
void Sprite_ClearPrimDepth(void *);
void Ui_DisableElementAlpha(void *, s32, s32);

void Ui_AddSprite(SpriteMgr *m, SpriteDesc *d) {
    s32 n = m->nSpr;
    SpriteEnt *e;
    TexSlot *t;
    s32 i;
#ifdef TARGET_PC
    PORT_DMA_BUF(ResInfo, resp, 0x2780); /* res below is *resp */
#define res (*resp)
#else
    ResInfo res;
#endif

    m->nSpr = n + 1;
    e = &m->spr[n];
    e->x0 = d->x4;
    e->x4 = d->x8;
    e->x8 = d->xC;
    e->xC = d->x10;
    t = NULL;
    for (i = 0; i < m->nTex; i++) {
        t = &m->tex[i];
        if (d->x14 == t->id) {
            break;
        }
    }
    if (i == m->nTex) {
        Res_Load(d->x18, &res);
        if (res.x8 == 0) {
            res.x8 = d->x14;
        }
        t = &m->tex[i];
        Tex_Init(t->tex, Res_Get(res.x8), res.x14, res.x18, res.xC, res.x10, 0xFC1034);
        m->nTex++;
        t->id = res.x8;
    }
    e->x10 = (TexInfo *)t->tex;
    e->x14 = Sprite_Create();
    e->x18 = d->x1C;
    e->x1C = d->x20;
    e->x20 = d->x24;
    e->x24 = d->x28;
    e->x28 = d->x2C;
    e->x2C = d->x30;
    e->x30 = d->x34;
    e->x34 = 0;
    e->x38 = 0;
    e->x3C = e->x10->xC;
    e->x40 = e->x10->x10;
    Sprite2D_InitObjs(e->x14, 3, e->x10, 0);
    if (d->x3C) {
        Sprite_SetPrimDepth(e->x14, d->x40, d->x44);
    } else {
        Sprite_ClearPrimDepth(e->x14);
    }
    Ui_DisableElementAlpha(gUi, 1, e->x0);
}
#ifdef TARGET_PC
#undef res
#endif
