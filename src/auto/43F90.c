#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u32 size; } HeapBlock;
typedef struct { s32 id; void *x4; u8 pad8[0x14]; } UiElem;
typedef struct {
    u8 pad[0x7588];
    UiElem elems[(0x8B5C - 0x7588) / 0x1C];
    u8 padE[(0x8B5C - 0x7588) % 0x1C];
    s16 count;
} Ui;
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
extern Ui *gUi[];
extern u8 gMainHeap[];
extern HeapBlock D_800BC9F8;
extern void *D_800BCA00;
void Ui_ClearDrawList(Ui *);
s32 Res_GetSize(s32);
void Res_Load(s32, void *);
void Heap_AllocAlt(void *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void *HeapBlock_Ptr(HeapBlock *);
void Ui_AddElement(Ui *, s32, s32);
void Game_SetNextMode(s32, s32);

void Pause_Return(Obj *o, s32 arg1) {
    void *p;
    void *t;
    UiElem *e = NULL;
    s32 i;
    s32 n;
    s32 size;

    Ui_ClearDrawList(gUi[0]);
    size = Res_GetSize(0x11424);
    t = &D_800BC9F8;
    Heap_AllocAlt(gMainHeap, t, size, 7, 0);
    p = HeapBlock_Ptr(t);
    Res_Load(0x11424, p);
    for (i = 0; i < gUi[0]->count; i++) {
        e = &gUi[0]->elems[i];
        if (e->id == 0x11424) {
            break;
        }
    }
    D_800BCA00 = e->x4;
    e->x4 = p;
    Ui_AddElement(gUi[0], 2, 0x39);
    Game_SetNextMode(0x1F, 1);
    o->x10 = arg1;
}
