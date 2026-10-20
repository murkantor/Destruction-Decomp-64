#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x20]; void *x20; u8 pad24[0x20]; } UiA;
typedef struct { void *x0; u8 pad4[0x20]; } UiB;
typedef struct { s32 x0; u8 pad4[0x18]; } UiC;
typedef struct {
    UiA a[1];
    u8 pad44[0x7588 - 0x44];
    UiC c[1];
    u8 pad75A4[0x8938 - 0x75A4];
    UiB b[1];
    u8 pad895C[0x8B5C - 0x895C];
    s16 x8B5C;
    s16 x8B5E;
    s16 x8B60;
    s16 x8B62;
    s16 x8B64;
    u8 pad8B66[2];
    u8 x8B68;
} Ui;
extern Ui *gUi[];
void Ui_ClearDrawList(Ui *);
void func_8005DCC4(Ui *);
void Ui_LoadResourceFile(Ui *, s32);
void Sprite_Destroy(void *);
void Res_Release(s32);

void Ui_ResetScreen(s32 a, s32 b) {
    s32 i;

    Ui_ClearDrawList(gUi[0]);
    func_8005DCC4(gUi[0]);
    gUi[0]->x8B68 = 0;
    for (i = 0; i < gUi[0]->x8B5E; i++) {
        Sprite_Destroy(gUi[0]->a[i].x20);
    }
    for (i = 0; i < gUi[0]->x8B62; i++) {
        Res_Release((s32)gUi[0]->b[i].x0);
    }
    for (i = 0; i < gUi[0]->x8B5C; i++) {
        Res_Release(gUi[0]->c[i].x0);
    }
    gUi[0]->x8B5C = 0;
    gUi[0]->x8B5E = 0;
    gUi[0]->x8B60 = 0;
    gUi[0]->x8B62 = 0;
    gUi[0]->x8B64 = 0;
    Ui_LoadResourceFile(gUi[0], a);
    Ui_LoadResourceFile(gUi[0], b);
}
