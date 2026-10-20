#include "common.h"
#include "heap.h"

#ifdef NON_MATCHING
/* Behaves like the original; written for the PC port from the assembly. */

/* First fit from the low end. alignOffset is where in the alignment the pointer must land. */
void Heap_Alloc(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset) {
    FreeBlock *prev = NULL;
    FreeBlock *blk;
    FreeBlock *rest;
    u32 pad;
    u32 have;

    if (size == 0) {
        goto fail;
    }
    size = (size + 3) & ~3;
    blk = heap->free;
    for (;;) {
        while (blk != NULL && blk->size < size) {
            prev = blk;
            blk = blk->next;
        }
        if (blk == NULL) {
            goto fail;
        }
        if (alignOffset < ((u32)blk & alignMask)) {
            pad = alignMask + alignOffset + 1 - ((u32)blk & alignMask);
        } else {
            pad = alignOffset - ((u32)blk & alignMask);
        }
        if (blk->size >= size + pad) {
            break;
        }
        prev = blk;
        blk = blk->next;
    }

    have = blk->size;
    if (have >= size + pad + 8) {
        rest = (FreeBlock *)((u8 *)blk + size + pad);
        rest->size = have - size - pad;
        rest->next = blk->next;
        if (prev != NULL) {
            prev->next = rest;
        } else {
            heap->free = rest;
        }
    } else {
        size = have - pad;
        if (prev != NULL) {
            prev->next = blk->next;
        } else {
            heap->free = blk->next;
        }
    }
    out->ptr = (u8 *)blk + pad;
    out->size = size;
    out->pad = pad;
    heap->bytesFree -= size + pad;
    heap->numAllocs++;
    if (heap->bytesFree < heap->minFree) {
        heap->minFree = heap->bytesFree;
    }
    return;

fail:
    out->ptr = NULL;
    out->size = 0;
    out->pad = 0;
}

/*
 * Last fit, taken from the top of the block. The original adds each candidate's
 * alignment trim to size as it goes, so size can grow past what was asked for;
 * that is kept.
 */
void Heap_AllocAlt(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset) {
    FreeBlock *prev = NULL;
    FreeBlock *best = NULL;
    FreeBlock *bestPrev = NULL;
    FreeBlock *cur;
    u32 pad = 0;
    u32 low;
    u32 have;

    if (size == 0) {
        goto fail;
    }
    for (cur = heap->free; cur != NULL; cur = cur->next) {
        if (cur->size < size) {
            prev = cur;
            continue;
        }
        low = ((u32)cur + cur->size - size) & alignMask;
        if (low < alignOffset) {
            size += low + alignMask - (alignOffset - 1);
        } else {
            size += low - alignOffset;
        }
        pad = ((u32)cur + cur->size - size) & 3;
        if (cur->size < size + pad) {
            prev = cur;
            continue;
        }
        best = cur;
        bestPrev = prev;
        prev = cur;
    }
    if (best == NULL) {
        goto fail;
    }

    have = best->size;
    if (have < size + pad + 8) {
        pad += have - size;
        if (bestPrev != NULL) {
            bestPrev->next = best->next;
        } else {
            heap->free = best->next;
        }
        out->ptr = (u8 *)best + pad;
    } else {
        best->size = have - (size + pad);
        out->ptr = (u8 *)best + best->size + pad;
    }
    out->size = size;
    out->pad = pad;
    heap->bytesFree -= size + pad;
    heap->numAllocs++;
    if (heap->bytesFree < heap->minFree) {
        heap->minFree = heap->bytesFree;
    }
    return;

fail:
    out->ptr = NULL;
    out->size = 0;
    out->pad = 0;
}

/* Put the block back in the address-sorted free list, merging with its neighbours. */
void Heap_Free(Heap *heap, HeapBlock *blk) {
    FreeBlock *prev = NULL;
    FreeBlock *cur;
    FreeBlock *f;
    FreeBlock *next;

    if (blk->ptr == NULL) {
        return;
    }
    blk->ptr = (u8 *)blk->ptr - blk->pad;
    blk->size = blk->size + blk->pad;
    blk->pad = 0;

    cur = heap->free;
    if (cur == NULL) {
        f = blk->ptr;
        f->size = blk->size;
        f->next = NULL;
        heap->free = f;
        goto done;
    }
    for (;;) {
        f = blk->ptr;
        if (cur < f) {
            if ((u8 *)cur + cur->size == (u8 *)f) {
                next = cur->next;
                cur->size += blk->size;
                if (next != NULL && (u8 *)cur + cur->size == (u8 *)next) {
                    cur->size += next->size;
                    cur->next = next->next;
                }
                goto done;
            }
        } else if (f < cur) {
            if ((u8 *)f + blk->size == (u8 *)cur) {
                f->size = blk->size + cur->size;
                f->next = cur->next;
            } else {
                f->size = blk->size;
                f->next = cur;
            }
            if (prev != NULL) {
                prev->next = f;
            } else {
                heap->free = f;
            }
            goto done;
        }
        prev = cur;
        cur = cur->next;
        if (cur == NULL) {
            break;
        }
    }
    f = blk->ptr;
    f->size = blk->size;
    f->next = NULL;
    prev->next = f;

done:
    heap->bytesFree += blk->size;
    heap->numAllocs--;
    blk->ptr = NULL;
    blk->size = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/heap", Heap_Alloc);

INCLUDE_ASM("asm/nonmatchings/heap", Heap_AllocAlt);

INCLUDE_ASM("asm/nonmatchings/heap", Heap_Free);
#endif

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
