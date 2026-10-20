#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    u8 pad8[0xC];
    s32 x14;
    s32 x18;
    f32 x1C;
    f32 x20;
    u8 pad24[0x24];
} Desc;
typedef struct { s32 a; s32 b; f32 x; f32 y; } Pos;
typedef struct { Pos p[2]; } Pos2;
typedef struct { u8 pad[0xC]; s16 xC; u8 padE[0x1058 - 0xE]; u64 x1058; } Game;
typedef struct { u64 t; u8 pad8[5]; u8 flag; } Timer;
typedef struct { s32 x0; u8 pad[0xC]; } TrackInfo;
typedef struct { s32 x0; u8 pad[0x64]; } Slot68;

extern Game *gGame;
extern void *gUi[];
extern u8 D_80097335;
extern s32 D_80097350;
extern u32 D_800958B4;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_80097CC8;
extern u64 D_80131298;
extern Timer D_8007EF20;
extern TrackInfo D_80095BE8[];
extern Slot68 D_80095908[];
extern Desc D_800BAE38;
extern Pos2 D_800BAE80;
extern u64 Time_Get(u64 *);
extern void Ui_ResetScreen(s32, s32);
extern void Race_PickRandomTrack(void);
extern void Ui_SaveElements(void);
extern void Ctx_FreeAll(void *);
extern void Ui_LoadResourceFile(void *, s32);
extern void Ui_AddModeLabels(s32);
extern void Ui_AddSpriteElement(void *, Desc *);
extern void Ui_AddElement(void *, s32, s32);
extern void Records_Draw(s32, s32, s32, s32);
extern void Ui_SetElementPos(void *, s32, s32, f32, f32);

static inline void AddSprite(void *ui, Desc *m) { Ui_AddSpriteElement(ui, m); }
static inline void AddEl(void *ui, s32 a, s32 b) { Ui_AddElement(ui, a, b); }
static inline void *GetUi(void) { return gUi[0]; }
void func_8004B048(void) {
    Desc m;
    Pos2 t;
    s32 mode;
    s32 sel;
    s32 n;
    s32 labels;
    s32 track;
    s32 i;

    sel = -1;
    n = 13;
    if (gGame->xC == 1) {
        if (D_80097335) {
            mode = 0x27;
        } else {
            mode = 0x19;
        }
    } else {
        mode = (gGame->xC == 2) ? 0x1F : 0x19;
    }
    if (mode == 0x19) {
        Ui_ResetScreen(0x112F0, 0x112F1);
        Race_PickRandomTrack();
        sel = 0;
        D_80097350 = 0;
        labels = D_800958B4 >= 4;
        track = D_800958B4;
        D_80131298 = Time_Get(&gGame->x1058) + 10000000000.0;
        D_8007EF20.flag = 0;
        D_8007EF20.t = Time_Get(&gGame->x1058) + 52000000.0;
    } else {
        Ui_SaveElements();
        Ctx_FreeAll(gUi[0]);
        Ui_LoadResourceFile(gUi[0], 0x112F0);
        Ui_LoadResourceFile(gUi[0], 0x112F1);
        if (D_800958A8 == 3 && D_800958AC == 1) {
            labels = 2;
            track = D_80097CC8;
            sel = n - 1;
        } else if (D_800958A8 == 3 && D_800958AC == 2) {
            labels = 3;
            track = 12;
            sel = n;
        } else if (D_800958B4 < 4) {
            labels = 0;
            track = D_800958B4;
            for (i = 0; i < 12; i++) {
                if (D_80095BE8[i].x0 == D_800958B4) {
                    sel = i;
                    break;
                }
            }
        } else {
            labels = 1;
            track = D_800958B4;
            for (i = 0; i < 12; i++) {
                if (D_80095BE8[i].x0 == D_800958B4) {
                    sel = i;
                    break;
                }
            }
        }
        D_80097350 = n + 1;
    }
    Ui_AddModeLabels(labels);
    m = D_800BAE38;
    t = D_800BAE80;
    m.x14 = track + 0x112D0;
    m.x4 = t.p[0].a;
    m.x18 = t.p[0].b;
    m.x1C = t.p[0].x;
    m.x20 = t.p[0].y;
    Ui_AddSpriteElement(GetUi(), &m);
    AddEl(gUi[0], 2, 0x12E);
    m.x14 = track + 0x112DD;
    m.x4 = t.p[1].a;
    m.x18 = t.p[1].b;
    m.x1C = t.p[1].x;
    m.x20 = t.p[1].y;
    Ui_AddSpriteElement(GetUi(), &m);
    AddEl(gUi[0], 2, 0x12F);
    Records_Draw(-1, labels, -1, track);
    {
        s32 off;
        for (off = 3 * sizeof(Slot68); off >= 0; off -= sizeof(Slot68)) {
            *(s32 *)((u8 *)D_80095908 + off) = sel;
        }
    }
    if (mode != 0x19) {
        Ui_AddElement(gUi[0], 2, 0x9F);
        Ui_AddElement(gUi[0], 2, 0xA0);
        if (sel == n) {
            Ui_SetElementPos(gUi[0], 2, 0xA0, -20.0f, 73.0f);
        } else if (sel == 0) {
            Ui_SetElementPos(gUi[0], 2, 0x9F, -20.0f, 73.0f);
        } else {
            Ui_SetElementPos(gUi[0], 2, 0x9F, 197.0f, 73.0f);
            Ui_SetElementPos(gUi[0], 2, 0xA0, 289.0f, 73.0f);
        }
    } else {
        Ui_AddElement(gUi[0], 2, 0x24);
    }
}
