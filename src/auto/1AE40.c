#include "common.h"
#include "m2c.h"

#include "heap.h"
typedef struct {
    s32 w;
    s32 h;
    u8 pad8[0x10];
    void *x18;
    HeapBlock h1C;
    s32 x24;
    s32 n;
    void *x2C;
    HeapBlock h30;
} Grid;
void *Mem_Alloc(u32);
void *HeapBlock_Ptr(HeapBlock *);

void Grid_Create(Grid **out, s32 n, s32 w, s32 h) {
    Grid *g = Mem_Alloc(sizeof(Grid));

    *out = g;
    g->w = w;
    (*out)->h = h;
    (*out)->n = n;
    (*out)->x24 = 0;
    Heap_Alloc(&gMainHeap, &(*out)->h1C, n * 2, 3, 0);
    (*out)->x18 = HeapBlock_Ptr(&(*out)->h1C);
    Heap_Alloc(&gMainHeap, &(*out)->h30, w * h * 8, 3, 0);
    (*out)->x2C = HeapBlock_Ptr(&(*out)->h30);
}
