typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    u8 padC[0xC];
    union { struct { s16 x18; s16 x1A; } s; f32 x18; } u18;
    f32 x1C;
    s32 x20;
    u8 pad24[0x80 - 0x24];
    s32 x80;
    s32 x84;
} UiItem;
typedef struct UiNode { UiItem *item; struct UiNode *next; } UiNode;
typedef struct {
    u8 pad0[0x8B54];
    UiNode *list;
    u8 pad8B58[0x8B68 - 0x8B58];
    u8 x8B68;
} UiMgr;
typedef struct { s32 x0; s32 x4; f32 x8; f32 xC; s32 x10; s32 x14; } UiSave;
extern UiMgr *gUi;
extern s32 gMainHeap;
extern s32 D_800BC9E8;
extern s32 D_800BC9F0;
extern s32 D_800BCA04;
extern s32 D_800BCA08;
void Heap_AllocAlt(void *, void *, s32, s32, s32);
void *HeapBlock_Ptr(void *);
void func_8005F884(void *);

static inline s32 UiSave_Fill(UiMgr *ui, UiSave *dst) {
    UiNode *node;
    s32 count;
    node = ui->list;
    count = 0;
    while (node != NULL) {
        UiItem *it = node->item;
        dst->x0 = it->x0;
        dst->x4 = it->x4;
        dst->x10 = it->x8;
        switch (it->x4) {
        case 1: {
            UiItem *p = node->item;
            dst->x8 = p->u18.x18;
            dst->xC = p->x1C;
            break;
        }
        case 2: {
            UiItem *p = node->item;
            dst->x8 = p->x80;
            dst->xC = p->x84;
            break;
        }
        case 0:
            it = node->item;
            dst->x8 = it->u18.s.x18;
            dst->xC = it->u18.s.x1A;
            dst->x14 = it->x20;
            break;
        }
        dst++;
        node = node->next;
        count++;
    }
    return count;
}

void func_800434B8(void) {
    D_800BCA04 = 0;
    D_800BCA08 = 0;
    if (gUi->list != NULL) {
        UiMgr *ui;
        UiSave *dst;
        Heap_AllocAlt(&gMainHeap, &D_800BC9E8, 0x960, 7, 0);
        ui = gUi;
        dst = HeapBlock_Ptr(&D_800BC9E8);
        D_800BCA04 = UiSave_Fill(ui, dst);
        if (gUi->x8B68) {
            func_8005F884(gUi);
            Heap_AllocAlt(&gMainHeap, &D_800BC9F0, 0x960, 7, 0);
            ui = gUi;
            dst = HeapBlock_Ptr(&D_800BC9F0);
            D_800BCA08 = UiSave_Fill(ui, dst);
        }
    }
}
