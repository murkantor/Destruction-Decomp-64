#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 x8[3]; u8 xB; } Cfg;
typedef struct { u8 pad[8]; } HeapBlock;
extern s32 gMainHeap;
extern u8 gParticlePool[];
typedef struct { u8 a[4]; } ViC;
extern ViC D_80123328;
extern u8 D_80131BA0[];
extern void func_80056310(void);
extern void func_80055334(void);
extern void func_80055320(void);
extern void func_80055390(void *, s32, s32);
extern void Heap_Alloc(void *, HeapBlock *, s32, s32, s32);
extern void *HeapBlock_Ptr(HeapBlock *);
extern s32 func_8005537C(HeapBlock *);
extern void Pool_Init(void *, s32, void *, s32);
extern void Vi_InitConfig(void *, u8);
extern void func_80055F70(void *);
extern void DebugMenu_BuildFrameSetup(void *);
static inline void heapinit(s32 a){ func_80055390(&gMainHeap, a, 0x80400000 - a); }
static inline void viinit(u8 b){ Vi_InitConfig(&D_80123328, b); }
s32 System_InitHeaps(Cfg *c) {
    HeapBlock blk;
    void *p;
    func_80056310();
    func_80055334();
    func_80055320();
    heapinit(c->x0);
    if (c->x4 > 0) {
        Heap_Alloc(&gMainHeap, &blk, c->x4 << 6, 0xF, 0);
        p = HeapBlock_Ptr(&blk);
        Pool_Init(gParticlePool, 0x40, p, func_8005537C(&blk));
    }
    viinit(c->xB);
    func_80055F70(D_80123328.a);
    DebugMenu_BuildFrameSetup(D_80131BA0);
    return 0;
}
