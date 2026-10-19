#ifndef HEAP_H
#define HEAP_H

#include "ultra64.h"

/* A free region, kept in a list sorted by address. */
typedef struct FreeBlock {
    /* 0x0 */ u32 size;
    /* 0x4 */ struct FreeBlock *next;
} FreeBlock;

typedef struct Heap {
    /* 0x00 */ FreeBlock *free;
    /* 0x04 */ u32 unk4;
    /* 0x08 */ u32 unk8;
    /* 0x0C */ u32 bytesFree;
    /* 0x10 */ s32 numAllocs;
    /* 0x14 */ u32 minFree;   /* lowest bytesFree seen */
} Heap;

/* An allocation: the pointer, and the size and alignment padding needed to free it. */
typedef struct HeapBlock {
    /* 0x0 */ void *ptr;
    /* 0x4 */ u32 size : 24;
    /* 0x7 */ u32 pad : 8;
} HeapBlock;

extern Heap gMainHeap;

void Heap_Alloc(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void Heap_AllocAlt(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void Heap_Free(Heap *heap, HeapBlock *blk);
void Heap_Shrink(Heap *heap, HeapBlock *blk, u32 size);

#endif
