#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u32 size; } HeapBlock;
typedef struct { void *a; void *b; } PtrPair;
typedef struct { HeapBlock a; HeapBlock b; } BlkPair;
typedef struct {
    u8 pad[0x2C];
    PtrPair ptrs[4];
    BlkPair blks[4];
    u8 pad8C[0x8C - 0x8C];
    s32 x8C;
    s32 x90;
    s32 x94;
} Obj;
extern u8 gMainHeap[];
extern s8 D_800D31CC[];
extern u8 D_80088E18[];
void Heap_Alloc(void *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void *HeapBlock_Ptr(HeapBlock *);
void func_80010950(Obj *, s32, void *, s32, s32, s32, s32, s32);

void func_800170F0(Obj *o, s32 n) {
    s32 i;

    o->x8C = 0;
    o->x90 = 100;
    o->x94 = n;
    for (i = 0; i < D_800D31CC[0]; i++) {
        Heap_Alloc(gMainHeap, &o->blks[i].a, (n * 3 + 0x50) * 8, 7, 0);
        o->ptrs[i].a = HeapBlock_Ptr(&o->blks[i].a);
        Heap_Alloc(gMainHeap, &o->blks[i].b, (n * 3 + 0x50) * 8, 7, 0);
        o->ptrs[i].b = HeapBlock_Ptr(&o->blks[i].b);
    }
    func_80010950(o, 1, D_80088E18, 0, 3, 0x20, 0x20, 6);
}
