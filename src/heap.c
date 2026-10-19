#include "common.h"
#include "heap.h"

INCLUDE_ASM("asm/nonmatchings/heap", Heap_Alloc);

INCLUDE_ASM("asm/nonmatchings/heap", Heap_AllocAlt);

INCLUDE_ASM("asm/nonmatchings/heap", Heap_Free);

/* Give back the end of an allocation, keeping its first size bytes. */
void Heap_Shrink(Heap *heap, HeapBlock *blk, u32 size) {
    HeapBlock tail;
    u32 cut;

    if (blk->ptr != NULL) {
        size = (size + 3) & ~3;
        if (size == 0) {
            Heap_Free(heap, blk);
        }
        cut = blk->size - size;
        if (cut >= 9) {
            tail.ptr = (u8 *)blk->ptr + size;
            tail.size = cut;
            tail.pad = 0;
            blk->size -= cut;
            Heap_Free(heap, &tail);
        }
    }
}
