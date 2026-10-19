typedef struct { u32 size : 24; u32 tag : 8; } BlockId;
typedef struct { void *ptr; BlockId id; } Block;
typedef struct Heap Heap;
extern Heap gMainHeap;
void Heap_Free(Heap *heap, Block *block);

static inline void Block_Shrink(u32 size, Block *blk) {
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
            blk->id.size -= extra;
            rest.ptr = (u8 *)blk->ptr + newSize;
            rest.id.size = extra;
            rest.id.tag = 0;
            Heap_Free(&gMainHeap, &rest);
        }
    }
}

void func_80055AEC(void *ptr, s32 size) {
    Block blk;
    BlockId *hdr;

    if (ptr != NULL) {
        hdr = (BlockId *)ptr - 1;
        blk.ptr = hdr;
        blk.id = *hdr;
        Block_Shrink(size, &blk);
        *hdr = blk.id;
    }
}
