#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[3]; s32 x14; s32 x18; f32 x1C; f32 x20; f32 x24_[5]; u8 x38; u8 pad39[3]; u8 x3C; u8 pad3D[3]; s32 x40; s32 x44; } UiElemDef; /* the real layout (as UiElemDesc in 3E0D4.c, names as this file uses them): the PC port converts the static descriptors at boot by their declared widths */
typedef struct { s32 a; s32 b; f32 x; f32 y; } Rec;
typedef struct { Rec r[2]; } Rec2;
typedef struct { s32 n; s32 ids[1]; } IdList;
typedef struct { s32 x0; u8 pad4[0x64]; } Ent68;
extern UiElemDef D_800BA3D0;
extern Rec2 D_800BA418;
extern IdList D_80125BE0;
extern s32 D_800958B4[];
extern Ent68 D_80095908[];
extern void *gUi[];
extern s32 D_80096E94;
extern s32 D_80096E90;
extern s32 D_80125C14;
extern s32 D_80097350;
void Ui_ResetScreen(s32, s32);
void Menu_BuildIdList(void);
void Ui_AddElement(void *, s32, s32);
void Ui_AddSpriteElement(void *, UiElemDef *);
void TrackMenu_Highlight(s32, s32);
void Cars_FreeObjects(void);

void TrackMenu_Show(void) {
    UiElemDef e;
    Rec2 r;
    s32 i;
    s32 j;

    e = D_800BA3D0;
    r = D_800BA418;
    Ui_ResetScreen(0x112B8, 0x112B9);
    Menu_BuildIdList();
    for (i = 0; i < D_80125BE0.n; i++) {
        if (D_80125BE0.ids[i] == D_800958B4[0]) {
            for (j = 3 * sizeof(Ent68); j >= 0; j -= sizeof(Ent68)) {
                *(s32 *)((u8 *)D_80095908 + j) = i;
            }
            break;
        }
    }
    Ui_AddElement(gUi[0], 2, 0x3F);
    Ui_AddElement(gUi[0], 2, 0x9D);
    e.x4 = r.r[0].a;
    e.x18 = r.r[0].b;
    e.x14 = D_800958B4[0] + 0x112D0;
    e.x1C = r.r[0].x;
    e.x20 = r.r[0].y;
    Ui_AddSpriteElement(gUi[0], &e);
    Ui_AddElement(gUi[0], 2, 0x12E);
    e.x4 = r.r[1].a;
    e.x18 = r.r[1].b;
    e.x14 = D_800958B4[0] + 0x112DD;
    e.x1C = r.r[1].x;
    e.x20 = r.r[1].y;
    Ui_AddSpriteElement(gUi[0], &e);
    Ui_AddElement(gUi[0], 2, 0x12F);
    D_80125C14 = D_800958B4[0];
    D_80097350 = D_80125BE0.n;
    Ui_AddElement(gUi[0], D_80096E94, D_80096E90);
    Ui_AddElement(gUi[0], 2, 0x9F);
    Ui_AddElement(gUi[0], 2, 0xA0);
    Ui_AddElement(gUi[0], 0, 0x17A);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_AddElement(gUi[0], 0, 0x17C);
    TrackMenu_Highlight(-1, D_800958B4[0]);
    Cars_FreeObjects();
}
