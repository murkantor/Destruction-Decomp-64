#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u32 size; } HeapBlock;
typedef struct { u8 pad[4]; } Heap;

typedef struct {
    u8 pad0[0x88];
    s32 x88;
    void *x8C;
    void *x90;
    s32 x94;
    s32 x98;
    s32 x9C;
    s32 xA0;
    s32 xA4;
    s32 xA8;
    s32 xAC;
    s32 xB0;
    s32 xB4;
    s32 xB8;
    s32 xBC;
} Ctx;

extern Heap gMainHeap;
extern u8 D_800BCC80[];
extern void *D_800B0B58;
extern s32 D_800BCCF4;
extern Ctx *D_800B0B54;
void Video_SetMode(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, f32, f32);
void Heap_Alloc(Heap *heap, HeapBlock *out, u32 size, s32 alignMask, s32 alignOffset);
void *HeapBlock_Ptr(HeapBlock *);
void Heap_Free(Heap *heap, HeapBlock *blk);
void func_80055F70(void *);
void func_80056098(void *);

Ctx Fb_CreateContext(void) {
    void *p;
    Ctx c;
    HeapBlock blk;

    Video_SetMode(D_800BCC80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0f, 1.0f);
    Heap_Alloc(&gMainHeap, &blk, 0x25800, 0x3F, 0);
    p = HeapBlock_Ptr(&blk);
    if (p == NULL) {
        p = D_800B0B58;
        if (p == NULL) {
            p = (u8 *)0x80400000 - D_800BCCF4;
        }
    }
    c.x88 = 0;
    c.x8C = D_800BCC80;
    c.x90 = p;
    c.x94 = 0;
    c.x98 = 0;
    c.x9C = 0;
    c.xA0 = 0;
    c.xA4 = 0;
    c.xA8 = 0;
    c.xAC = 0;
    c.xB4 = 0;
    c.xB8 = 0;
    D_800B0B54 = &c;
    if (c.x90 != NULL) {
        D_800B0B58 = c.x90;
    }
    func_80055F70(D_800BCC80);
    func_80056098(c.x90);
    Heap_Free(&gMainHeap, &blk);
    return c;
}
