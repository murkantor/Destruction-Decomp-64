#include "common.h"
#include "m2c.h"

typedef struct { u8 data[8]; } HeapBlock;
typedef struct { void *a; void *b; } BufPair;
typedef struct { HeapBlock a; HeapBlock b; } BlockPair;
typedef struct {
    u8 pad0[0x2C];
    BufPair bufs[4];
    BlockPair blocks[4];
    u8 x8C;
    u8 pad8D[3];
    s32 x90;
    u8 pad94[0xC];
    s32 xA0;
} Obj174D0;
extern u8 gMainHeap[];
extern s8 D_800D31CC[];
extern u8 D_8008EE20[];
void Heap_Alloc(void *, HeapBlock *, s32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
void func_80010950(Obj174D0 *, s32, void *, s32, s32, s32, s32, s32);

void Particles_InitTrail(Obj174D0 *obj, s32 n) {
    s32 i;

    obj->x8C = 0;
    obj->x90 = 0;
    obj->xA0 = n;
    for (i = 0; i < D_800D31CC[0]; i++) {
        Heap_Alloc(gMainHeap, &obj->blocks[i].a, n * 16 + 0x140, 7, 0);
        obj->bufs[i].a = HeapBlock_Ptr(&obj->blocks[i].a);
        Heap_Alloc(gMainHeap, &obj->blocks[i].b, n * 16 + 0x140, 7, 0);
        obj->bufs[i].b = HeapBlock_Ptr(&obj->blocks[i].b);
    }
    func_80010950(obj, 1, D_8008EE20, 0, 2, 0x20, 0x20, 1);
}
