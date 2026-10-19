#include "common.h"
#include "m2c.h"

typedef struct HeapBlock {
    void *ptr;
    u32 size : 24;
    u32 pad : 8;
} HeapBlock;

typedef struct Node {
    void *elem;
    struct Node *next;
} Node;

typedef struct {
    u8 pad0[0x8B54];
    Node *list;
} Ui;

typedef struct {
    s16 type;
    u8 pad2[2];
    s32 x4;
    u8 pad8[0x19];
    u8 x21;
    u8 pad22[2];
} Entry24;

typedef struct {
    s16 type;
    u8 pad2[2];
    s32 x4;
    s32 x8;
    u8 padC[0x2C];
    u8 x38;
    u8 pad39[0xF];
} Entry48;

extern u8 gMainHeap[];

u32 Res_GetSize(s32);
void Res_Load(s32, void *);
void Heap_AllocAlt(void *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void Heap_Free(void *heap, HeapBlock *blk);
void *HeapBlock_Ptr(HeapBlock *);
void *Ui_FindElement(Ui *, s32, s32);
void *Mem_Alloc(u32);
void Ui_AddMissingResources(Ui *, void *, u32);
void func_8005EE90(Ui *, Entry24 *);
void Ui_AddSprite(Ui *, Entry48 *);
void Ui_AddSpriteElement(Ui *, Entry48 *);
void Ui_LoadElements(Ui *, void *, u32);

static inline void Ui_AddToList(Ui *ui, s32 kind, s32 id) {
    Node *p;
    Node *node;
    Node *node1;
    void *elem;
    void *elem0;

    if (ui->list == NULL) {
        elem0 = Ui_FindElement(ui, kind, id);
        node = Mem_Alloc(8);
        if (node != NULL) {
            node->elem = elem0;
            node->next = ui->list;
            ui->list = node;
        }
    } else {
        p = ui->list;
        while (p->next != NULL) {
            p = p->next;
        }
        elem = Ui_FindElement(ui, kind, id);
        node1 = Mem_Alloc(8);
        if (node1 != NULL) {
            node1->elem = elem;
            node1->next = NULL;
            p->next = node1;
        }
    }
}

s32 func_8005DD20(Ui *ui, s32 id) {
    HeapBlock blk;
    u32 size;
    void *buf;
    s32 i;
    s32 n;

    size = Res_GetSize(id);
    if (size == 0) {
        return 0;
    }
    Heap_AllocAlt(gMainHeap, &blk, size, 7, 0);
    buf = HeapBlock_Ptr(&blk);
    Res_Load(id, buf);
    switch (*(s16 *)buf) {
    case 0:
        break;
    case 0x3F0:
        Ui_AddMissingResources(ui, buf, size);
        break;
    case 0x3F1: {
        Entry24 *e;
        n = size / sizeof(Entry24);
        for (i = 0; i < n; i++) {
            e = &((Entry24 *)buf)[i];
            func_8005EE90(ui, e);
            if (e->x21) {
                Ui_AddToList(ui, 0, e->x4);
            }
        }
        break;
    }
    case 0x3F2: {
        Entry48 *e = buf;
        n = size / sizeof(Entry48);
        for (i = 0; i < n; e++, i++) {
            switch (e->x8) {
            case 1:
                Ui_AddSprite(ui, e);
                break;
            case 2:
                Ui_AddSpriteElement(ui, e);
                break;
            }
            if (e->x38) {
                Ui_AddToList(ui, e->x8, e->x4);
            }
        }
        break;
    }
    case 0x3F3:
        Ui_LoadElements(ui, buf, size);
        break;
    }
    Heap_Free(gMainHeap, &blk);
    return 0;
}
