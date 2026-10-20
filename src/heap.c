#include "common.h"
#include "heap.h"

void Heap_Alloc(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset) {
    FreeBlock *prev;
    FreeBlock *b;
    u32 pad;
    FreeBlock *nb;
    u32 min;
    u32 sz;

    if (size == 0) {
        goto fail;
    }
    sz = (size + 3) & ~3;
    prev = NULL;
    b = heap->free;
    for (;;) {
        if (b == NULL) {
            goto fail;
        }
        while (b != NULL && b->size < sz) {
            prev = b;
            b = b->next;
        }
        if (b == NULL) {
            goto fail;
        }
        if (alignOffset < ((u32)b & alignMask)) {
            pad = alignMask + 1 + alignOffset - ((u32)b & alignMask);
        } else {
            pad = alignOffset - ((u32)b & alignMask);
        }
        if (b->size >= sz + pad) {
            break;
        }
        prev = b;
        b = b->next;
    }
    if (b->size >= sz + pad + 8) {
        nb = (FreeBlock *)((u8 *)b + sz + pad);
        nb->size = b->size - sz - pad;
        nb->next = b->next;
        if (prev != NULL) {
            prev->next = nb;
        } else {
            heap->free = nb;
        }
    } else {
        sz = b->size - pad;
        if (prev != NULL) {
            prev->next = b->next;
        } else {
            heap->free = b->next;
        }
    }
    out->ptr = (u8 *)b + pad;
    out->size = sz;
    out->pad = pad;
    goto done;
fail:
    out->ptr = NULL;
    out->size = 0;
    out->pad = 0;
    return;
done:
    PORT_MARK_RAW(out->ptr, sz); /* a fresh block holds nothing: no stale big-endian marks */
    heap->bytesFree -= sz + pad;
    heap->numAllocs++;
    min = heap->minFree;
    if (heap->bytesFree < min) {
        min = heap->bytesFree;
    }
    heap->minFree = min;
}

void Heap_AllocAlt(Heap *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset) {
    FreeBlock *prev;
    FreeBlock *b;
    FreeBlock *best;
    FreeBlock *bestPrev;
    u32 pad;
    u32 adj;
    u32 min;

    if (size == 0) {
        goto fail;
    }
    pad = 0;
    prev = NULL;
    best = NULL;
    bestPrev = NULL;
    for (b = heap->free; b != NULL; b = b->next) {
        if (b->size >= size) {
            adj = ((u32)b + b->size - size) & alignMask;
            if (adj < alignOffset) {
                size += adj + (alignMask + 1 - alignOffset);
            } else {
                size += adj - alignOffset;
            }
            pad = ((u32)b + b->size - size) & 3;
            if (b->size >= size + pad) {
                best = b;
                bestPrev = prev;
            }
        }
        prev = b;
    }
    if (best == NULL) {
        goto fail;
    }
    if (best->size >= size + pad + 8) {
        best->size -= size + pad;
        out->ptr = (u8 *)best + best->size + pad;
        out->size = size;
        out->pad = pad;
    } else {
        pad += best->size - size;
        if (bestPrev != NULL) {
            bestPrev->next = best->next;
        } else {
            heap->free = best->next;
        }
        out->ptr = (u8 *)best + pad;
        out->size = size;
        out->pad = pad;
    }
    goto done;
fail:
    out->ptr = NULL;
    out->size = 0;
    out->pad = 0;
    return;
done:
    PORT_MARK_RAW(out->ptr, size); /* a fresh block holds nothing: no stale big-endian marks */
    heap->bytesFree -= size + pad;
    heap->numAllocs++;
    min = heap->minFree;
    if (heap->bytesFree < min) {
        min = heap->bytesFree;
    }
    heap->minFree = min;
}

void Heap_Free(Heap *heap0, HeapBlock *blk) {
    Heap *heap = heap0;
    FreeBlock *f;
    FreeBlock *prev;
    FreeBlock *p;
    FreeBlock *nb;

    if (blk->ptr == NULL) {
        return;
    }
    blk->ptr = (u8 *)blk->ptr - blk->pad;
    blk->size += blk->pad;
    blk->pad = 0;
    f = heap->free;
    prev = NULL;
    if (f == NULL) {
        nb = blk->ptr;
        nb->size = blk->size;
        nb->next = NULL;
        heap->free = nb;
        goto done;
    }
    do {
        p = blk->ptr;
        if (f < p) {
            if ((FreeBlock *)((u8 *)f + f->size) == p) {
                f->size += blk->size;
                if (f->next != NULL && (FreeBlock *)((u8 *)f + f->size) == f->next) {
                    f->size += ((FreeBlock *)((u8 *)f + f->size))->size;
                    f->next = f->next->next;
                }
                goto done;
            }
        } else if (p < f) {
            if ((FreeBlock *)((u8 *)p + blk->size) == f) {
                p->size = blk->size + f->size;
                p->next = f->next;
            } else {
                p->size = blk->size;
                p->next = f;
            }
            if (prev != NULL) {
                prev->next = p;
            } else {
                heap->free = p;
            }
            goto done;
        }
        prev = f;
        f = f->next;
    } while (f != NULL);
    nb = blk->ptr;
    nb->size = blk->size;
    nb->next = NULL;
    prev->next = nb;
done:
    heap->bytesFree += blk->size;
    heap->numAllocs--;
    blk->ptr = NULL;
    blk->size = 0;
}

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
