#include "common.h"
#include "m2c.h"

typedef struct { s32 id; void *x4; u8 pad[0x14]; } TexSlot;
typedef struct {
    u8 pad0[0x7588];
    TexSlot tex[184];
    u8 pad2[0x8B5C - 0x7588 - 184 * 0x1C];
    s16 nTex;
} UiMgr;
typedef struct { u8 pad[0xC]; s16 mode; } Game;
typedef struct { u8 pad[0x510]; s32 *x510; f32 x514; } SndCfg;
typedef struct { u8 pad[0x10]; s32 x10; } MenuItem;
extern Game *gGame;
extern UiMgr *gUi;
extern UiMgr *gUiA[] __asm__("gUi");
extern s32 D_800BC9E4;
extern SndCfg *D_8011EEF8;
extern u8 D_80096EB4;
extern s32 gMainHeap;
extern s32 D_800BC9F8;
extern struct { u8 pad[8]; s32 blk; } D_800BC9F0;
extern void *D_800BCA00;
extern s32 D_8011EEE0[];
extern s32 D_800958A8;
extern f64 D_800BADE8;
s32 Sound_CountPlaying(s32);
void Res_LoadSoundBank(s32);
void Ui_ClearDrawList(void *);
s32 Res_GetSize(s32);
void Heap_AllocAlt(void *, void *, s32, s32, s32);
void *HeapBlock_Ptr(void *);
void Res_Load(s32, void *);
void Ui_AddElement(void *, s32, s32);
void Game_SetNextMode(s32, s32);
void Sound_SetVolume(s32, s32);

s32 OptionsMenu_Back(MenuItem *it) {
    s32 next;
    s16 mode = gGame->mode;

    if (mode == 1) {
        next = 0x13;
    } else if (mode == 2) {
        next = 0x1F;
    } else {
        next = 0x13;
    }
    if (Sound_CountPlaying(2) == 0) {
        s32 id;
        if (D_800BC9E4 == 12) {
            id = 0x759E;
        } else if (D_800BC9E4 == 13) {
            id = 0x75A0;
        } else {
            id = D_8011EEF8->x510[D_800BC9E4];
        }
        Res_LoadSoundBank(id);
        D_80096EB4 = 0;
    }
    if (next == 0x1F) {
        TexSlot *t;
        void *p;
        s32 i;
        s32 st;
        t = NULL;
        Ui_ClearDrawList(gUi);
        Heap_AllocAlt(&gMainHeap, &D_800BC9F8, Res_GetSize(0x11424), 7, 0);
        p = HeapBlock_Ptr(&D_800BC9F0.blk);
        st = 2;
        Res_Load(0x11424, p);
        for (i = 0; i < gUi->nTex; i++) {
            t = &gUi->tex[i];
            if (t->id == 0x11424) {
                break;
            }
        }
        D_800BCA00 = t->x4;
        t->x4 = p;
        Ui_AddElement(gUiA[0], 2, 0x39);
        Game_SetNextMode(0x1F, 1);
        it->x10 = st;
        Sound_SetVolume(D_8011EEE0[0], 0);
    } else {
        f32 v = D_8011EEF8->x514 * D_800BADE8;
        if (v < 0.0) {
            v = 0.0f;
        }
        Sound_SetVolume(D_8011EEE0[0], (s32)v);
        Game_SetNextMode(0x13, 1);
        it->x10 = D_800958A8;
    }
    return 1;
}
