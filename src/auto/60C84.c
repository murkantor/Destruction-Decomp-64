#include "common.h"
#include "m2c.h"

typedef struct { s32 a; s32 b; } HeapBlock;
typedef struct {
    u8 pad[0x84];
    s32 x84;
    s32 x88;
    u8 pad8C[0xC4];
    HeapBlock x150;
    HeapBlock x158;
    HeapBlock x160;
} Obj;
typedef struct { u8 pad[8]; s32 x8; } InitConfig;
extern InitConfig gInitConfig;
extern u8 gMainHeap[];
void func_80055CD0(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, f32, f32);
void Heap_Free(void *, HeapBlock *);
void Heap_Alloc(void *, HeapBlock *, s32, s32, s32);
s32 HeapBlock_Ptr(HeapBlock *);

s32 Obj_ReallocBuffers(Obj *obj, s32 mode, s32 arg2, s32 arg3) {
    s32 p;

    func_80055CD0((u8 *)obj + 0x10, gInitConfig.x8, arg3, 1, 0, arg2, 0, 0, 1, 0, 1.0f, 1.0f);
    Heap_Free(gMainHeap, &obj->x150);
    Heap_Alloc(gMainHeap, &obj->x150, obj->x88, 0x3F, 0);
    p = HeapBlock_Ptr(&obj->x158);
    if (p != HeapBlock_Ptr(&obj->x160)) {
        Heap_Free(gMainHeap, &obj->x160);
    }
    Heap_Free(gMainHeap, &obj->x158);
    Heap_Alloc(gMainHeap, &obj->x158, obj->x84, 0x3F, 0);
    if (mode == 2) {
        Heap_Alloc(gMainHeap, &obj->x160, obj->x84, 0x3F, 0);
    } else {
        obj->x160 = obj->x158;
    }
    return 0;
}
