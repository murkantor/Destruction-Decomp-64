#include "common.h"
#include "m2c.h"

typedef struct { u8 data[8]; } HeapBlock;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; } Cfg;
typedef struct { u8 pad[0xC]; s32 xC; s32 x10; } Sprite;
typedef struct { s32 id; Sprite spr; u8 pad[0x1C - 4 - sizeof(Sprite)]; } Slot;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    u8 xC;
    u8 padD[3];
    Sprite *x10;
    u8 pad14[4];
    u8 x18[0x70 - 0x18];
    s32 x70;
    s32 x74;
    f32 x78;
    f32 x7C;
    s32 x80;
    s32 x84;
    s32 x88;
    s32 x8C;
} UiElemB;
typedef struct {
    u8 pad0[0x670];
    UiElemB b[(0x6BB0 - 0x670) / 0x90];
    u8 padB[0x7588 - 0x670 - ((0x6BB0 - 0x670) / 0x90) * 0x90];
    Slot slots[(0x8B5C - 0x7588) / 0x1C];
    u8 padS[0x8B5C - 0x7588 - ((0x8B5C - 0x7588) / 0x1C) * 0x1C];
    s16 slotCount;
    s16 pad8B5E;
    s16 countB;
} Ui;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    u8 x10;
    u8 pad11[3];
    s32 x14;
    s32 x18;
    f32 x1C;
    f32 x20;
    u8 pad24[0x2C - 0x24];
    f32 x2C;
    f32 x30;
    u8 pad34[0x3C - 0x34];
    u8 x3C;
    u8 pad3D[3];
    s32 x40;
    s32 x44;
} Desc;
extern u8 gMainHeap[];
extern void *gUi;
void Heap_AllocAlt(void *, HeapBlock *, s32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
void Heap_Free(void *, HeapBlock *);
void Res_Load(s32, void *);
s32 Res_Get(s32);
void Tex_Init(void *, s32, s32, s32, s32, s32, s32);
void Bg_Init(void *, Sprite *, s32);
void func_80061524(void *, s32, s32);
void func_8006153C(void *);
void func_80061590(void *, s32);
void func_80061588(void *, s32);
void Bg_DisableAlpha(void *);
u8 *Ui_FindElement(void *, s32, s32);

void Ui_AddSpriteElement(Ui *ui, Desc *d) {
    HeapBlock blk;
    char unused[8];
    Cfg *cfg;
    UiElemB *el;
    Slot *slot;
    s32 i;

    Heap_AllocAlt(gMainHeap, &blk, 0x1C, 7, 0);
    cfg = HeapBlock_Ptr(&blk);
    {
        s32 n = ui->countB;
        ui->countB = n + 1;
        el = &ui->b[n];
    }
    el->x0 = d->x4;
    el->x4 = d->x8;
    el->x8 = d->xC;
    el->xC = d->x10;
    i = 0;
    slot = NULL;
    for (; i < ui->slotCount; i++) {
        slot = &ui->slots[i];
        if (d->x14 == slot->id) {
            break;
        }
    }
    if (i == ui->slotCount) {
        Res_Load(d->x18, cfg);
        if (cfg->x8 == 0) {
            cfg->x8 = d->x14;
        }
        slot = &ui->slots[i];
        Tex_Init(&slot->spr, Res_Get(cfg->x8), cfg->x14, cfg->x18, cfg->xC, cfg->x10, 0xFFF4);
        ui->slotCount++;
        slot->id = cfg->x8;
    }
    el->x10 = &slot->spr;
    el->x70 = 0;
    el->x74 = 0;
    el->x78 = d->x2C;
    el->x7C = d->x30;
    el->x80 = d->x1C;
    el->x84 = d->x20;
    el->x88 = el->x10->xC;
    el->x8C = el->x10->x10;
    Bg_Init(el->x18, el->x10, 0);
    if (d->x3C) {
        func_80061524(el->x18, d->x40, d->x44);
    } else {
        func_8006153C(el->x18);
    }
    func_80061590(el->x18, 8);
    func_80061588(el->x18, 0x2000);
    Bg_DisableAlpha(Ui_FindElement(gUi, 2, el->x0) + 0x18);
    Heap_Free(gMainHeap, &blk);
}
