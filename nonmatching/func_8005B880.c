typedef struct { s32 x0; s32 x4; s32 x8; } Cfg;
typedef struct { u8 pad[0x10]; } HeapBlock;
extern s32 D_80130250;
extern s32 gControllersLatched;
extern void *D_801238E4;
extern s32 D_80131BA0;
extern s32 gMainHeap;
extern s32 gParticlePool;
extern s32 D_80123328;
void osCreateScheduler(void *, void *, s32, s32, s32);
void *osScGetCmdQ(void *);
void func_80055FFC(s32);
void func_80055320(void);
void DebugMenu_Init(void *);
void func_80055390(void *, s32, s32);
void Heap_Alloc(void *, HeapBlock *, u32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
u32 func_8005537C(HeapBlock *);
void Pool_Init(void *, u32, void *, u32);
void Vi_InitConfig(void *, u8);
void func_80055F70(void *);
void func_80053D30(void *);
void DebugMenu_BuildVideo(void *);
void DebugMenu_BuildFrameSetup(void *);
void func_800575A0(void *);
void func_8005A960(void *);
void Pad_InitAll(void *);

s32 func_8005B880(s32 pri, Cfg *cfg) {
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
    func_80055FFC(1);
    func_80055320();
    DebugMenu_Init(&D_80131BA0);
    func_80055390(&gMainHeap, cfg->x0, 0x80400000 - cfg->x0);
    if (cfg->x4 > 0) {
        void *ptr;
        Heap_Alloc(&gMainHeap, &blk, cfg->x4 << 6, 0xF, 0);
        ptr = HeapBlock_Ptr(&blk);
        Pool_Init(&gParticlePool, 0x40, ptr, func_8005537C(&blk));
    }
    Vi_InitConfig(&D_80123328, ((u8 *)cfg)[0xB]);
    func_80055F70(&D_80123328);
    func_80053D30(&D_80131BA0);
    DebugMenu_BuildVideo(&D_80131BA0);
    DebugMenu_BuildFrameSetup(&D_80131BA0);
    func_800575A0(&D_80131BA0);
    func_8005A960(&D_80131BA0);
    Pad_InitAll(&D_80131BA0);
    return 0;
}
