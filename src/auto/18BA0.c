#include "common.h"
#include "m2c.h"

typedef struct { u8 data[8]; } HeapBlock;
typedef struct { void *a; void *b; HeapBlock blkA; HeapBlock blkB; } Pool;
typedef struct { u8 x0; u8 pad1[0x27]; s32 x28; } Ent2C;
typedef struct {
    u8 x0;
    u8 pad1[3];
    s32 x4;
    s32 x8;
    Pool pool1;
    u8 pad24[0x114 - 0x24];
    Pool pool2;
    u8 pad12C[0x21C - 0x12C];
    HeapBlock blk;
    Ent2C *x224;
    s32 x228;
    u8 pad22C[0x2B0 - 0x22C];
} Mgr;
typedef struct { u8 pad[4]; u8 x4; } Grid;
typedef struct { u8 pad[0xC]; Grid *grid; } Track;
typedef struct { u8 pad[0x70]; Track *track; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;
extern Game *gGame;
extern u8 gMainHeap[];
extern s16 D_800BC9C8;
extern Grid *D_800BC9C4;
extern u8 *D_800BC9C0;
void *Mem_Alloc(s32);
void Heap_Alloc(void *, HeapBlock *, u32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
void func_80018D70(void);

void Shadow_InitMgr(Mgr **pp) {
    Pool *pool;
    s32 i;

    *pp = Mem_Alloc(sizeof(Mgr));
    D_800BC9C4 = gGame->race->track->grid;
    D_800BC9C0 = &gGame->race->track->grid->x4;
    D_800BC9C8 = 40;
    Heap_Alloc(gMainHeap, &(*pp)->blk, 0x6E0, 7, 0);
    (*pp)->x224 = HeapBlock_Ptr(&(*pp)->blk);
    pool = &(*pp)->pool1;
    Heap_Alloc(gMainHeap, &pool->blkA, D_800BC9C8 * sizeof(u64) * 2.1, 7, 0);
    Heap_Alloc(gMainHeap, &pool->blkB, D_800BC9C8 * 0x30, 0xF, 0);
    pool->a = HeapBlock_Ptr(&pool->blkA);
    pool->b = HeapBlock_Ptr(&pool->blkB);
    pool = &(*pp)->pool2;
    Heap_Alloc(gMainHeap, &pool->blkA, D_800BC9C8 * sizeof(u64) * 2.1, 7, 0);
    Heap_Alloc(gMainHeap, &pool->blkB, D_800BC9C8 * 0x30, 0xF, 0);
    pool->a = HeapBlock_Ptr(&pool->blkA);
    pool->b = HeapBlock_Ptr(&pool->blkB);
    (*pp)->x0 = 0;
    (*pp)->x4 = 0;
    (*pp)->x228 = 0;
    (*pp)->x8 = 0;
    for (i = 0; i < 40; i++) {
        (*pp)->x224[i].x0 = 0;
        (*pp)->x224[i].x28 = 0;
    }
    func_80018D70();
}
