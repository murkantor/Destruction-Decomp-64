#include "common.h"
#include "pool.h"

void Pool_Init(Pool *pool, u32 itemSize, void *buf, u32 bufSize) {
    u32 count = bufSize / itemSize;
    u8 *p;
    u8 *next;
    u32 i;

    pool->itemSize = itemSize;
    p = buf;
    next = (u8 *)buf + itemSize;
    for (i = 0; i < count - 1; i++) {
        *(u8 **)p = next;
        p = next;
        next = p + itemSize;
    }
    *(u8 **)p = NULL;
    pool->free = buf;
    pool->base = buf;
    pool->size = bufSize;
    pool->sizeFree = bufSize;
}

void Pool_GetFree(Pool *pool, s32 *bytes, s32 *count) {
    void **p = pool->free;
    s32 n = 0;

    if (p != NULL) {
        do {
            p = *p;
            n += 1;
        } while (p != NULL);
    }
    *bytes = n * pool->itemSize;
    *count = n;
}

void *Pool_Alloc(Pool *pool) {
    void **item = pool->free;

    if (item == NULL) {
        return NULL;
    }
    pool->free = *item;
    pool->sizeFree -= pool->itemSize;
    return item;
}

void Pool_Free(Pool *pool, void *item) {
    if (item != NULL) {
        *(void **)item = pool->free;
        pool->free = item;
        pool->sizeFree += pool->itemSize;
    }
}
