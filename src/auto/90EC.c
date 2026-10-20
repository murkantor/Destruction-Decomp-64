#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u32 size; } HeapBlock;
typedef struct { s16 x0; s16 x2; s32 x4; s32 x8; } ResHdr;
typedef struct { u8 pad[0xC]; s32 xC[0x3D]; s32 x100; u8 pad2[0x514 - 0x104]; f32 x514; u8 pad3[0x528 - 0x518]; u8 x528; } State;
extern u8 gMainHeap[];
extern u8 D_80116DF8[];
extern s32 D_8011EEE0[];
extern State *D_8011EEF8;
extern u32 Res_GetSize(s32);
extern void Res_Load(s32, void *);
extern void Heap_AllocAlt(void *, HeapBlock *, u32, u32, u32);
extern void *HeapBlock_Ptr(HeapBlock *);
extern void Heap_Free(void *, HeapBlock *);
extern void Sound_FadeOutAll(s32, s32);
extern s32 Sound_CountPlaying(s32);
extern void Rom_Copy(s32, void *, s32);
extern s32 Seq_StartWithBank(void *, void *);
extern s32 Sound_SetVolume(s32, s32);
extern s32 SoundEmitter_Init(s32, void *);
extern void func_80064100(s32);

s32 Res_LoadSoundBank(ResHdr *p) {
    HeapBlock blk; u32 size; s32 i; s32 n; f32 vol;

    size = Res_GetSize((s32)p);
    if (size != 0) {
        Heap_AllocAlt(gMainHeap, &blk, size, 7, 0);
        { ResHdr *q = p; p = HeapBlock_Ptr(&blk); Res_Load((s32)q, p); }
        switch (p->x0) {
        case 0:
            break;
        case 0x3F4:
            n = size / 44;
            for (i = 0; i < n; i++) {
                func_80064100(SoundEmitter_Init(D_8011EEF8->xC[D_8011EEF8->x100], p));
                p = (ResHdr *)((u8 *)p + 44);
            }
            break;
        case 13:
            Sound_FadeOutAll(2, 0);
            while (Sound_CountPlaying(2) != 0) {
            }
            Rom_Copy(p->x4, D_80116DF8, p->x8 - p->x4);
            D_8011EEE0[0] = Seq_StartWithBank(D_80116DF8 - 25000, D_80116DF8);
            vol = D_8011EEF8->x514 * 128.0;
            Sound_SetVolume(D_8011EEE0[0], vol);
            D_8011EEF8->x528 = Sound_CountPlaying(2);
            break;
        }
        Heap_Free(gMainHeap, &blk);
    }
    return 0;
}
