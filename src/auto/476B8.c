#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[3]; s32 x14; s32 x18; f32 x1C[7]; u8 x38; u8 pad39[3]; u8 x3C; u8 pad3D[3]; s32 x40; s32 x44; } UiElemDesc; /* the real layout (Ui_AddSpriteElement reads x10, x38 and x3C as bytes): the PC port converts these static descriptors at boot by their declared widths */
typedef struct { s32 x0; s32 x4; u8 pad8[0x14]; } UiItem;
typedef struct { s32 x0; s32 x4; u8 pad8[0x24]; } SpriteInfo;

typedef struct {
    u8 pad0[0x7588];
    UiItem items[1];
} UiA;
typedef struct {
    u8 pad0[0x8B5C];
    s16 nItems;
    u8 pad8B5E[0xA];
    u8 x8B68;
} Ui;
typedef struct {
    u8 pad0[0x1058];
    u8 x1058[0x28];
    u8 x1080[0x28];
    u8 x10A8[8];
} Game;

extern Ui *gUi[];
extern Game *gGame[];
extern u8 gMainHeap[];
extern s32 D_80095908;
extern u8 D_800BC9E0;
extern s32 D_800BCA00;
extern s32 D_80125C38;
extern s32 D_800958B4;
extern UiElemDesc D_800BAA80;
extern UiElemDesc D_800BAAC8;
extern SpriteInfo D_80096EDC[];
extern u8 D_800BC9E8[];
extern u8 D_800BC9F0[];
extern u8 D_800BC9F8[];
extern f32 D_80125BD4;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958A4[];
extern char gSaveSlotText[][50];
extern s32 D_80097350;
extern s32 D_8011EEF8;
extern s32 D_8011EEE0;
extern s8 D_800D31CC;

void Announcer_ClearQueue(void);
void Sound_FadeOutAll(s32, s32);
s32 Sound_CountPlaying(s32);
void Rumble_AllowRetry(void);
void Ui_AddElement(void *, s32, s32);
void Ui_AddSpriteElement(void *, UiElemDesc *);
void Ctx_FreeAll(void *);
void Hud_GetLayoutIds(s32 *);
void Ui_LoadResourceFile(void *, s32);
void MapUi_Init(void *);
void Ui_RestoreElements(void);
void Heap_Free(void *, void *);
f32 Time_ToSeconds(void *);
u64 Time_Get(void *);
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
void Clock_Pause(void *);
void Clock_Resume(void *);
void func_8005CC4C(s32);
void Sound_SetVolume(s32, s32);
void Pad_Rumble(s32, s32, f32);

static inline void AddSprites(void)
{
        UiElemDesc a;
        UiElemDesc b;
        s32 k = D_800958B4;
        a = D_800BAA80;
        b = D_800BAAC8;
        a.x14 = D_80096EDC[k].x0;
        a.x18 = D_80096EDC[k].x4;
        Ui_AddSpriteElement(gUi[0], &b);
        Ui_AddSpriteElement(gUi[0], &a);
}

void Pause_Init(void)
{
    s32 lim[2];
    UiItem *item = NULL;
    s32 i;

    Announcer_ClearQueue();
    Sound_FadeOutAll(2, 0);
    while (Sound_CountPlaying(2) != 0) {
    }
    Rumble_AllowRetry();
    if ((u32)(D_80095908 - 2) >= 2) {
        if (D_80095908 != 1) {
            Ui_AddElement(gUi[0], 2, 0x39);
        }
        Ui_AddElement(gUi[0], 2, 0x38);
        Ui_AddElement(gUi[0], 0, 0x1A7);
        Ui_AddElement(gUi[0], 0, 0x1A6);
        Ui_AddElement(gUi[0], 0, 0x1B0);
        Ui_AddElement(gUi[0], 0, 0x1B1);
        D_800BC9E0 = gUi[0]->x8B68;
    } else {
        for (i = 0; i < gUi[0]->nItems; i++) {
            item = &((UiA *)gUi[0])->items[i];
            if (item->x0 == 0x11424)
                break;
        }
        item->x4 = D_800BCA00;
        Ctx_FreeAll(gUi[0]);
        Hud_GetLayoutIds(lim);
        Ui_LoadResourceFile(gUi[0], lim[0]);
        Ui_LoadResourceFile(gUi[0], lim[1]);
        MapUi_Init(gUi[0]);
        if (D_80125C38 == 0x1A) {
            AddSprites();
        }
        Ui_RestoreElements();
        gUi[0]->x8B68 = D_800BC9E0;
        Heap_Free(gMainHeap, D_800BC9E8);
        Heap_Free(gMainHeap, D_800BC9F0);
        Heap_Free(gMainHeap, D_800BC9F8);
        D_80125BD4 = Time_ToSeconds(gGame[0]->x1058) + 1.0f;
        if (D_800958A8 == 3 && D_800958AC == 2) {
            u64 limit = 300000000000ULL;
            u64 t = Time_Get(gGame[0]->x10A8);
            char buf[8];
            s32 base = 9;
            if (t >= limit)
                t = 0;
            else
                t = limit - t;
            Time_Format(Time_Split(t), buf, 8);
            for (i = 0; i < D_800958A4[0]; i++) {
                gSaveSlotText[base + i][0] = buf[0];
                gSaveSlotText[base + i][1] = buf[1];
                gSaveSlotText[base + i][2] = buf[2];
                gSaveSlotText[base + i][3] = buf[3];
                gSaveSlotText[base + i][4] = 0;
            }
        }
    }
    D_80097350 = 4;
    Clock_Pause(gGame[0]->x1058);
    Clock_Resume(gGame[0]->x1080);
    func_8005CC4C(D_8011EEF8);
    Sound_SetVolume(D_8011EEE0, 0);
    for (i = 0; i < D_800D31CC; i++) {
        Pad_Rumble(i, 0, 0.0f);
        Pad_Rumble(i, 0, 0.0f);
        Pad_Rumble(i, 0, 0.0f);
    }
}
