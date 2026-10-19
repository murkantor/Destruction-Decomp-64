typedef struct { u32 a; u32 b; } HeapBlock;
typedef struct { void *p0; void *p4; HeapBlock b8; HeapBlock b10; } Sub;
typedef struct { s32 x0; Sub x4; u8 pad1C[0xF0]; Sub x10C; u8 pad124[0x1A4]; } Obj;
typedef struct { u8 pad0[0xC]; u8 *xC; } T70;
typedef struct { u8 pad0[0x70]; T70 *x70; } T1100;
typedef struct { u8 pad0[0x1100]; T1100 *x1100; } Game;
extern Game *gGame;
extern s16 D_800BC9A8;
extern u8 *D_800BC9A4;
extern u8 *D_800BC9A0;
extern u8 gMainHeap[];
void *Mem_Alloc(u32 size);
void Heap_Alloc(void *heap, HeapBlock *out, u32 size, u32 alignMask, u32 alignOffset);
void *HeapBlock_Ptr(HeapBlock *);
void func_80018D70(void);

void Obj_AllocSubBuffers(Obj **arg0) {
    Sub *s;

    *arg0 = Mem_Alloc(sizeof(Obj));
    D_800BC9A8 = 0x28;
    D_800BC9A4 = gGame->x1100->x70->xC;
    D_800BC9A0 = gGame->x1100->x70->xC + 4;
    (*arg0)->x0 = 0;
    s = &(*arg0)->x4;
    Heap_Alloc(gMainHeap, &(*arg0)->x4.b8, 0x2A0, 7, 0);
    Heap_Alloc(gMainHeap, &s->b10, D_800BC9A8 * 0x30, 0xF, 0);
    s->p0 = HeapBlock_Ptr(&s->b8);
    s->p4 = HeapBlock_Ptr(&s->b10);
    s = &(*arg0)->x10C;
    Heap_Alloc(gMainHeap, &(*arg0)->x10C.b8, (u32)((u32)(D_800BC9A8 * 8) * 2.1), 7, 0);
    Heap_Alloc(gMainHeap, &s->b10, D_800BC9A8 * 0x30, 0xF, 0);
    s->p0 = HeapBlock_Ptr(&s->b8);
    s->p4 = HeapBlock_Ptr(&s->b10);
    func_80018D70();
}
