#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 pad[0x60]; } Ent68;
typedef struct { u8 pad[0x40]; s32 x40; u8 pad44[0xC]; } Ent50;
typedef struct { s16 x0; s32 x4; } Ent8;
typedef struct { u8 x0; u8 x1; u8 pad[0xA]; } EntC;
typedef struct { u8 pad[0x504]; Ent50 *x504; s32 x508; } Obj96F4;
typedef struct {
    u8 pad[0x50C];
    s32 *x50C;
    s32 *x510;
    u8 pad514[0xC];
    s32 x520;
    u8 pad524[8];
    s32 x52C;
    u8 x530;
} Settings;
typedef struct { u8 pad[0xC]; s16 xC; u8 padE[0x10FC - 0xE]; s16 x10FC; } Game;

extern Ent68 D_8009592C[];
extern u64 gGameTime;
extern u64 D_801244D8;
extern s32 D_80124364;
extern s32 D_8013137C;
extern s32 D_80126068[];
extern s32 D_80125F58[];
extern u64 D_800D12E0[];
extern u8 D_80124368[];
extern Game *gGame;
extern Settings *D_8011EEF8[];
extern volatile s32 D_800958B4;
extern Ent8 D_8007EDD8[];
extern Ent8 D_8007EE28[];
extern EntC D_8007EE00[];

void *Mem_Alloc(s32);
u64 Time_Get(u64 *);
void Game_SaveItems(Settings *);
s32 Sound_CountPlaying(s32);
void func_800084EC(s32);
void Owner_AllocSlotBuffers(Obj96F4 *);
void Sound_StartCarLoops(Settings *);

void func_800096F4(Obj96F4 *obj) {
    s32 i;
    s32 j;
    Ent50 *e;
    s32 snd;
    s32 st;
    s32 *tbl;
    Settings *set;

    for (i = 0; i < 4; i++) {
        D_8009592C[i].x0 = 0;
        D_8009592C[i].x4 = 0;
    }
    e = obj->x504 = Mem_Alloc(0x280);
    for (i = 7; i >= 0; i--) {
        e->x40 = 0;
        e++;
    }
    obj->x508 = 0;
    D_801244D8 = Time_Get(&gGameTime);
    D_80124364 = 0;
    D_8013137C = -1;
    for (j = 0; j < 15; j++) {
        D_80126068[j] = -1;
        D_80125F58[j] = 0;
    }
    for (i = 0; i < 0xF8; i++) {
        D_800D12E0[i] = D_801244D8;
        D_80124368[i] = 0;
    }
    switch (gGame->xC) {
    case 0:
        Game_SaveItems(D_8011EEF8[0]);
        break;
    case 1:
        Game_SaveItems(D_8011EEF8[0]);
        if (D_8011EEF8[0]->x530 == 0 && gGame->x10FC != 3 && Sound_CountPlaying(2) == 0) {
            func_800084EC(0x75A0);
            D_8011EEF8[0]->x520 = 13;
        }
        break;
    case 2:
        Owner_AllocSlotBuffers(obj);
        tbl = D_8011EEF8[0]->x50C; func_800084EC(tbl[D_800958B4]);
        Game_SaveItems(D_8011EEF8[0]);
        if (D_8011EEF8[0]->x530 == 0) {
            if (gGame->x10FC == 25) {
                snd = 0x759E;
                st = 12;
                D_8011EEF8[0]->x530 = 1;
            } else {
                tbl = D_8011EEF8[0]->x510; snd = tbl[D_800958B4];
                st = D_800958B4;
            }
            if (Sound_CountPlaying(2) != 0) {
                func_800084EC(snd);
                D_8011EEF8[0]->x52C = 0;
            } else {
                D_8011EEF8[0]->x52C = snd;
                D_8011EEF8[0]->x520 = st;
            }
        }
        Sound_StartCarLoops(D_8011EEF8[0]);
        for (i = 0; i < 4; i++) {
            D_8007EDD8[i].x0 = 0;
            D_8007EDD8[i].x4 = 0;
        }
        for (i = 0; i < 4; i++) {
            D_8007EE28[i].x0 = 0;
            D_8007EE28[i].x4 = 0;
        }
        for (i = 0; i < 4; i++) {
            D_8007EE00[i].x1 = 0;
            D_8007EE00[i].x0 = 0;
        }
        break;
    }
}
