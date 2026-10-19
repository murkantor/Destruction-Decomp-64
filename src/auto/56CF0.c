#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u32 size; } HeapBlock;
extern u8 gMainHeap[];
extern HeapBlock D_800BCEF4, D_800BCEFC, D_800BCF04;
extern HeapBlock D_800BCF98[2];
extern void *D_800BCEE8, *D_800BCEEC, *D_800BCEF0;
extern OSMesgQueue D_800BCF18, D_800BCF58;
extern OSMesg D_800BCF30[], D_800BCF70[];
extern u8 D_80130250[];
extern u8 D_800BCF0C[];
extern u8 D_800BCD00[];
extern char D_800BBBC0[], D_800BBBCC[], D_800BBBD8[], D_800BBBE0[];
extern s32 gVideoInterlace, gVideoDither, gVideoFifoShift;
extern u8 D_800B0B2C[], D_800B0B38[];
extern void Heap_Alloc(void *, HeapBlock *, u32, u32, u32);
extern void *HeapBlock_Ptr(HeapBlock *);
extern void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
extern void osSetEventMesg(s32, OSMesgQueue *, OSMesg);
extern void osScAddClient(void *, void *, OSMesgQueue *);
extern void DebugMenu_AddSubmenu(void *, void *, void *);
extern void DebugMenu_Init(void *);
extern void DebugMenu_AddInt(void *, void *, void *, s32, s32, void *);

void DebugMenu_BuildFrameSetup(void *menu) {
    Heap_Alloc(gMainHeap, &D_800BCEF4, 0x400, 0xF, 0);
    D_800BCEE8 = HeapBlock_Ptr(&D_800BCEF4);
    Heap_Alloc(gMainHeap, &D_800BCEFC, 0x2080, 0xF, 0);
    D_800BCEEC = HeapBlock_Ptr(&D_800BCEFC);
    Heap_Alloc(gMainHeap, &D_800BCF04, 0xC00, 0xF, 0);
    D_800BCEF0 = HeapBlock_Ptr(&D_800BCF04);
    osCreateMesgQueue(&D_800BCF18, D_800BCF30, 10);
    osCreateMesgQueue(&D_800BCF58, D_800BCF70, 10);
    osSetEventMesg(14, &D_800BCF58, D_800BCF70[0]);
    osScAddClient(D_80130250, D_800BCF0C, &D_800BCF18);
    Heap_Alloc(gMainHeap, &D_800BCF98[0], 0x1000, 0xF, 0);
    Heap_Alloc(gMainHeap, &D_800BCF98[1], 0x1000, 0xF, 0);
    DebugMenu_AddSubmenu(menu, D_800BBBC0, D_800BCD00);
    DebugMenu_Init(D_800BCD00);
    DebugMenu_AddInt(D_800BCD00, D_800BBBCC, &gVideoInterlace, 0, 2, D_800B0B2C);
    DebugMenu_AddInt(D_800BCD00, D_800BBBD8, &gVideoDither, 0, 3, D_800B0B38);
    DebugMenu_AddInt(D_800BCD00, D_800BBBE0, &gVideoFifoShift, 0, 0x20, NULL);
}
