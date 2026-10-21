#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; } Cfg;
typedef struct { u8 pad[0x8]; } HeapBlock;
extern s32 D_80130250;
extern s32 gControllersLatched;
extern void *D_801238E4;
extern s32 D_80131BA0;
extern s32 gMainHeap;
extern s32 gParticlePool;
extern s32 D_80123328;
void osCreateScheduler(void *, void *, s32, s32, s32);
void *osScGetCmdQ(void *);
void Vi_InitManager(s32);
void func_80055320(void);
void DebugMenu_Init(void *);
void Heap_Init(void *, s32, s32);
void Heap_Alloc(void *, HeapBlock *, u32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
u32 HeapBlock_Size(HeapBlock *);
void Pool_Init(void *, u32, void *, u32);
void Vi_InitConfig(void *, u8);
void Vi_ApplyConfig(void *);
void DebugMenu_Reset(void *);
void DebugMenu_BuildVideo(void *);
void DebugMenu_BuildFrameSetup(void *);
void Font_InitDebug(void *);
void func_8005A960(void *);
void Pad_InitAll(void *);

static inline void DM_Init(void *a) { DebugMenu_Init(a); }
static inline void HeapInitW(void *a, s32 b, s32 c) { Heap_Init(a, b, c); }
static inline void Vi_Init(void *a, u8 b) { Vi_InitConfig(a, b); }
static inline void F70(void *a) { Vi_ApplyConfig(a); }
static inline void D30(void *a) { DebugMenu_Reset(a); }
static inline void BV(void *a) { DebugMenu_BuildVideo(a); }
static inline void BF(void *a) { DebugMenu_BuildFrameSetup(a); }
static inline void A0(void *a) { Font_InitDebug(a); }
static inline void A960(void *a) { func_8005A960(a); }
static inline void PI(void *a) { Pad_InitAll(a); }
static inline void *HBP(HeapBlock *b) { return HeapBlock_Ptr(b); }
static inline u32 S7C(HeapBlock *b) { return HeapBlock_Size(b); }
static inline void HA(void *a, HeapBlock *b, u32 c, s32 d, s32 e) { Heap_Alloc(a, b, c, d, e); }
s32 Main_InitSystems(s32 pri, Cfg *cfg) {
    HeapBlock blk;
    switch (cfg->x8) {
    case 0:
        osCreateScheduler(&D_80130250, &gControllersLatched, pri, 2, 1);
        break;
    case 1:
        osCreateScheduler(&D_80130250, &gControllersLatched, pri, 0x10, 1);
        break;
    case 2:
        osCreateScheduler(&D_80130250, &gControllersLatched, pri, 0x1E, 1);
        break;
    }
    D_801238E4 = osScGetCmdQ(&D_80130250);
    Vi_InitManager(1);
    func_80055320();
    DM_Init(&D_80131BA0);
#ifdef TARGET_PC
    {
        extern u8 gRamEnd[]; /* where 0x80400000 is in the port's RAM image */
        HeapInitW(&gMainHeap, cfg->x0, (s32)gRamEnd - cfg->x0);
    }
#else
    HeapInitW(&gMainHeap, cfg->x0, 0x80400000 - cfg->x0);
#endif
    if (cfg->x4 > 0) {
        void *ptr;
#ifdef TARGET_PC
        /* the drones: the matrices and viewports of a frame come from this pool,
           so it grows with the cars (Port_FieldPoolScale: 1 = the N64's) */
        s32 Port_FieldPoolScale(void);
        Heap_Alloc(&gMainHeap, &blk, (cfg->x4 * Port_FieldPoolScale()) << 6, 0xF, 0);
#else
        Heap_Alloc(&gMainHeap, &blk, cfg->x4 << 6, 0xF, 0);
#endif
        ptr = HeapBlock_Ptr(&blk);
        Pool_Init(&gParticlePool, 0x40, ptr, HeapBlock_Size(&blk));
    }
    Vi_Init(&D_80123328, ((u8 *)cfg)[0xB]);
    F70(&D_80123328);
    D30(&D_80131BA0);
    BV(&D_80131BA0);
    BF(&D_80131BA0);
    A0(&D_80131BA0);
    A960(&D_80131BA0);
    PI(&D_80131BA0);
    return 0;
}
