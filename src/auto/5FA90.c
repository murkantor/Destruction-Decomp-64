#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    u8 xC;
    void *x10;
    s32 x14;
    u16 x18;
    u16 x1A;
    u8 x1C;
    s32 x20;
} Entry;

typedef struct {
    s32 x0;
    u8 x4[0x20];
} Res;

typedef struct {
    s32 key;
    s32 val;
} Pair;

typedef struct {
    u8 pad0[4];
    s32 x4;
    s32 x8;
    s32 xC;
    u8 x10;
    s32 x14;
    s32 x18;
    u16 x1C;
    u16 x1E;
    u8 x20;
} Def;

typedef struct {
    u8 pad0[0x6BB0];
    Entry ent[(0x8938 - 0x6BB0) / 0x24];
    Res res[15];
    u8 pad8B54[4];
    Pair *x8B58;
    u8 pad8B5C[6];
    s16 x8B62;
    s16 x8B64;
    s16 x8B66;
} Mgr;

typedef struct { u8 pad[0x10]; void *x10; } UiElem;

extern void *gUi[];
void Res_Load(s32, void *);
void Ui_AddResource(Mgr *, void *);
UiElem *Ui_FindElement(void *, s32, s32);
void Font_DisableAlpha(void *);

void Ui_AddTextElement(Mgr *m, Def *d) {
    s32 buf[10];
    Entry *e;
    s32 i;

    e = &m->ent[m->x8B64++];
    e->x0 = d->x4;
    e->x4 = d->x8;
    e->x8 = d->xC;
    e->xC = d->x10;
    for (i = 0; i < m->x8B66; i++) {
        if (d->x4 == m->x8B58[i].key) {
            e->x14 = m->x8B58[i].val;
            break;
        }
    }
    for (i = 0; i < m->x8B62; i++) {
        if (d->x14 == m->res[i].x0) {
            break;
        }
    }
    if (i == m->x8B62) {
        Res_Load(d->x18, buf);
        Ui_AddResource(m, buf);
    }
    e->x10 = m->res[i].x4;
    e->x18 = d->x1C;
    e->x1A = d->x1E;
    e->x1C = d->x20;
    e->x20 = d->x14;
    Font_DisableAlpha(Ui_FindElement(gUi[0], 0, e->x0)->x10);
}
