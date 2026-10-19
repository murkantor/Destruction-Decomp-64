#ifndef POOL_H
#define POOL_H

#include "ultra64.h"

/* Fixed-size blocks carved out of one buffer, linked through their first word when free. */
typedef struct Pool {
    /* 0x00 */ void *free;      /* first free block */
    /* 0x04 */ u32 itemSize;
    /* 0x08 */ void *base;
    /* 0x0C */ u32 size;
    /* 0x10 */ u32 sizeFree;
} Pool;

void Pool_Init(Pool *pool, u32 itemSize, void *buf, u32 bufSize);
void Pool_GetFree(Pool *pool, s32 *bytes, s32 *count);
void *Pool_Alloc(Pool *pool);
void Pool_Free(Pool *pool, void *item);

#endif
