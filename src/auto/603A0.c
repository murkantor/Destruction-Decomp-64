#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 x8; u8 x9; u8 xA; u8 xB; void *xC; u16 x10; u16 x12; u8 x14; u8 pad[3]; s32 x18; } EntSub;
typedef struct { s32 id; EntSub s; u8 pad[4]; } Entry;
typedef struct { u8 pad[0x8938]; Entry e[15]; u8 pad2[0xE]; s16 count; } Ui;
typedef struct { u8 pad[4]; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[5]; u16 x16; u8 pad18[2]; u16 x1A; u8 x1C; u8 pad1D[6]; u8 x23; u8 pad24[3]; u8 x27; } Def;
extern void *Res_Get(s32);
static inline void fill(EntSub *s, u8 a, u8 b, void *r, u16 g, u16 f, s32 c, u8 e, u8 h, s32 i) {
    s->x9 = a;
    s->xA = b;
    s->xC = r;
    s->x4 = c;
    s->x8 = e;
    s->x12 = f;
    s->x10 = g;
    s->x14 = h;
    s->x18 = i;
}
void Ui_AddResource(Ui *ui, Def *d) {
    s32 n = ui->count;
    u8 a = d->x23;
    u8 b = d->x27;
    void *r = Res_Get(d->x4);
    fill(&ui->e[n].s, a, b, r, d->x16, d->x1A, d->xC, d->x10, d->x1C, d->x8);
    ui->e[n].id = d->x4;
    ui->count++;
}
