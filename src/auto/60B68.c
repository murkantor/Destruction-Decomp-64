#include "common.h"
#include "m2c.h"

typedef struct { u32 x0; u32 x4; } HeapBlock;
typedef struct { u8 pad[0x8]; s32 x8; } InitConfig;
typedef struct {
    u16 x0;
    u16 x2;
    void *x4;
    u8 pad8[0x10 - 0x8];
    u8 x10[0x84 - 0x10];
    s32 x84;
    s32 x88;
    u8 pad8C[0x150 - 0x8C];
    HeapBlock x150;
    HeapBlock x158[2];
    u8 pad168[0x178 - 0x168];
    HeapBlock x178[2];
    u8 pad188[0x56C - 0x188];
    u8 x56C;
} AudioObj;
extern InitConfig gInitConfig;
extern u8 gMainHeap[];
extern s32 D_80124350;
u16 Obj_NextId(void);
void Video_SetMode(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, f32, f32);
void Heap_Alloc(void *heap, HeapBlock *out, u32 size, s32 alignMask, s32 alignOffset);

s32 Render_Init(AudioObj *o) {
    s32 i;
    o->x2 = Obj_NextId();
    o->x0 = 0x3EA;
    o->x4 = o;
    o->x56C = 0;
    D_80124350 = 0;
    Video_SetMode(o->x10, gInitConfig.x8, 0, 1, 0, 0, 0, 0, 1, 0, 1.0f, 1.0f);
    Heap_Alloc(gMainHeap, &o->x150, o->x88, 0x3F, 0);
    for (i = 0; i < 2; i++) {
        Heap_Alloc(gMainHeap, &o->x158[i], o->x84, 0x3F, 0);
#ifdef TARGET_PC
        {
            /* the frame's display list: it grows with the drones' cars
               (Port_FieldPoolScale: 1 = the N64's) */
            s32 Port_FieldPoolScale(void);
            Heap_Alloc(gMainHeap, &o->x178[i], 0xBB80 * Port_FieldPoolScale(), 7, 0);
        }
#else
        Heap_Alloc(gMainHeap, &o->x178[i], 0xBB80, 7, 0);
#endif
    }
    return 0;
}
