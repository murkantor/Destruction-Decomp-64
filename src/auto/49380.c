#include "common.h"
#include "m2c.h"

typedef struct { s32 v[3]; } Ids3;
typedef struct { f32 v[3]; } Vol3;

typedef struct {
    u8 pad[0x514];
    Vol3 vol;
    s32 x520;
    u8 x524;
} Options;

typedef struct {
    u8 pad[0xC];
    s16 xC;
} Game;

typedef struct {
    u8 pad[0x88];
    s32 x88;
} UiElem;

typedef struct {
    s32 x0;
    u8 pad[0x64];
} Slot;

extern Options *D_8011EEF8;
extern Ids3 D_800BAD40;
extern Game *gGame;
extern void *gUi[];
extern s32 D_80097350[];
extern s32 D_8011EEE0;
extern char D_80123D82[];
extern char D_800BA87C[];
extern s32 D_800BC9E4;
extern Slot D_80095908[];

void Ui_SaveElements(void);
void Ctx_FreeAll(void *);
void func_8005DD20(void *, s32);
void Sound_SetVolume(s32, s32);
void Ui_AddElement(void *, s32, s32);
UiElem *Ui_FindElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);

static inline Vol3 GetVol(void) {
    Vol3 r;
    r.v[0] = D_8011EEF8->vol.v[0];
    r.v[1] = D_8011EEF8->vol.v[1];
    r.v[2] = D_8011EEF8->vol.v[2];
    return r;
}

void func_80048780(void) {
    Ids3 ids = D_800BAD40;
    Vol3 vol = GetVol();
    s32 mode;
    f32 f;
    f32 v;
    s32 i;
    s32 j;
    UiElem *e;

    if (gGame->xC == 1) {
        mode = 0x13;
    } else if (gGame->xC == 2) {
        mode = 0x1F;
    } else {
        mode = 0x13;
    }
    if (mode == 0x1F) {
        Ui_SaveElements();
    }
    Ctx_FreeAll(gUi[0]);
    func_8005DD20(gUi[0], 0x11330);
    func_8005DD20(gUi[0], 0x11331);
    D_80097350[0] = 5;
    f = D_8011EEF8->vol.v[0] * 128.0;
    if (f < 0.0) {
        f = 0.0f;
    }
    Sound_SetVolume(D_8011EEE0, f);
    if (D_8011EEF8->vol.v[0] == 0.0) {
        Ui_AddElement(gUi[0], 2, 0x5A);
    } else if (D_8011EEF8->vol.v[0] == 2.0) {
        Ui_AddElement(gUi[0], 2, 0x5B);
    }
    if (D_8011EEF8->vol.v[1] == 0.0) {
        Ui_AddElement(gUi[0], 2, 0x5D);
    } else if (D_8011EEF8->vol.v[1] == 2.0) {
        Ui_AddElement(gUi[0], 2, 0x5E);
    }
    if (D_8011EEF8->vol.v[2] == 0.0) {
        Ui_AddElement(gUi[0], 2, 0x60);
    } else if (D_8011EEF8->vol.v[2] == 2.0) {
        Ui_AddElement(gUi[0], 2, 0x61);
    }
    Ui_AddElement(gUi[0], 2, 0x59);
    sprintf(D_80123D82, D_800BA87C, D_8011EEF8->x520 + 1);
    D_800BC9E4 = D_8011EEF8->x520;
    if (D_8011EEF8->x524) {
        Ui_AddElement(gUi[0], 2, 0x64);
    } else {
        Ui_AddElement(gUi[0], 2, 0x63);
    }
    for (i = 0; i < 3; i++) {
        v = vol.v[i];
        e = Ui_FindElement(gUi[0], 2, ids.v[i]);
        e->x88 = v * 72.5f + 2.0f;
        if (v == 1.0) {
            e->x88++;
        }
    }
    for (j = 0x138; j >= 0; j -= 0x68) {
        *(s32 *)((u8 *)D_80095908 + j) = 0;
    }
}
