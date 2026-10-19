#ifndef MEM_H
#define MEM_H

#include "ultra64.h"

/* What the heap returns: the block and the id it needs back to free it. */
typedef struct HeapBlock {
    /* 0x0 */ void *ptr;
    /* 0x4 */ s32 id;
} HeapBlock;

typedef struct Heap Heap;

extern Heap gMainHeap;

void Heap_Alloc(Heap *heap, HeapBlock *out, u32 size, s32 alignMask, s32 alignOffset);
void Heap_AllocAlt(Heap *heap, HeapBlock *out, u32 size, s32 alignMask, s32 alignOffset);
void Heap_Free(Heap *heap, HeapBlock *block);

/* Allocations from gMainHeap keep the block id in the word before the pointer. */
void *Mem_Alloc(u32 size);
void *Mem_AllocAlt(u32 size);
void *Mem_AllocAligned(u32 size, s32 alignMask, s32 alignOffset);
void *Mem_AllocAlignedAlt(u32 size, s32 alignMask, s32 alignOffset);
void Mem_Free(void *ptr);

#endif
