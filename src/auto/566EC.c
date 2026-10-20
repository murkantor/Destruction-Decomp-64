#include "common.h"
#include "m2c.h"

#ifdef HOST_LE
typedef struct { u32 tag : 8; u32 size : 24; } BlockId;
#else
typedef struct { u32 size : 24; u32 tag : 8; } BlockId;
#endif
typedef struct { void *ptr; BlockId id; } Block;
typedef struct Heap Heap;
extern Heap gMainHeap;
void Heap_Free(Heap *heap, Block *block);

static inline void Block_Shrink(Block *blk, u32 size) {
    Block rest;
    u32 newSize;
    u32 extra;

    if (blk->ptr != NULL) {
        newSize = (size + 7) & ~3;
        if (newSize == 0) {
            Heap_Free(&gMainHeap, blk);
        }
        extra = blk->id.size - newSize;
        if (extra > 8) {
rest.ptr = (u8 *)blk->ptr + newSize; rest.id.size = extra; blk->id.size -= extra; rest.id.tag = 0;
            Heap_Free(&gMainHeap, &rest);
        }
    }
}

void Mem_Shrink(void *ptr, u32 size) {
    Block blk;
    BlockId *hdr;

    if (ptr != NULL) {
        hdr = ptr;
        hdr--;
        blk.ptr = hdr;
        blk.id = *hdr;
        Block_Shrink(&blk, size);
        *hdr = blk.id;
    }
}
