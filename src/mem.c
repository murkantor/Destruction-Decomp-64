#include "common.h"
#include "mem.h"

void *Mem_Alloc(u32 size) {
    HeapBlock blk;
    s32 *p;

    Heap_Alloc(&gMainHeap, &blk, size + 4, 3, 0);
    p = blk.ptr;
    *p = blk.id;
    return p + 1;
}

void *Mem_AllocAlt(u32 size) {
    HeapBlock blk;
    s32 *p;

    Heap_AllocAlt(&gMainHeap, &blk, size + 4, 3, 0);
    p = blk.ptr;
    *p = blk.id;
    return p + 1;
}

void *Mem_AllocAligned(u32 size, s32 alignMask, s32 alignOffset) {
    HeapBlock blk;
    s32 *p;

    Heap_Alloc(&gMainHeap, &blk, size + 4, alignMask, (alignOffset - 4) & alignMask);
    p = blk.ptr;
    if (p == NULL) {
        return NULL;
    }
    *p = blk.id;
    return p + 1;
}

void *Mem_AllocAlignedAlt(u32 size, s32 alignMask, s32 alignOffset) {
    HeapBlock blk;
    s32 *p;

    Heap_AllocAlt(&gMainHeap, &blk, size + 4, alignMask, (alignOffset - 4) & alignMask);
    p = blk.ptr;
    if (p == NULL) {
        return NULL;
    }
    *p = blk.id;
    return p + 1;
}

void Mem_Free(void *ptr) {
    HeapBlock blk;

    if (ptr != NULL) {
        blk.ptr = (s32 *)ptr - 1;
        blk.id = ((s32 *)ptr)[-1];
        Heap_Free(&gMainHeap, &blk);
    }
}
