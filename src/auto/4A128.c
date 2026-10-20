#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { char c[2]; } S2;
typedef struct { char c[8]; } S8;
typedef struct { char name[0xC]; s32 score; s32 x10; } Thr;
typedef struct {
    char n0[9];
    char n1[9];
    char n2[9];
    u8 pad1B[5];
    s32 x20;
    u8 pad24[4];
    u64 x28;
    u64 x30;
} Record;
typedef struct { s32 id; s32 res; u8 pad8[0x14]; } UiElem;
typedef struct {
    u8 pad0[0x7588];
    UiElem elems[199];
    u8 pad8B4C[0x10];
    s16 count;
} Ui;

extern S2 D_800BAF30;
extern S8 D_800BAF34;
extern Ui *gUi[];
extern s32 D_80097250[];
extern s32 D_8009725C;
extern s32 D_80097224[];
extern char D_80123DB4[][0x32];
extern Thr D_80097CCC[];
extern Record D_80097A28[];
extern char D_800BAF68[];
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
char *strcpy(char *, const char *);
s32 sprintf(char *, const char *, ...);
f32 Font_RightAlignX(char *, s32, f32);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void Res_Release(s32);
s32 Res_Get(s32);

#define FIND_ELEM(id_)                                   \
    for (i = 0; i < gUi[0]->count; i++) {                \
        if ((id_) == gUi[0]->elems[i].id) {                   \
            el = &gUi[0]->elems[i];                      \
            break;                                       \
        }                                                \
    }

void Records_Draw(s32 prev, s32 mode, s32 base, s32 cup) {
    S2 e = D_800BAF30;
    S8 buf = D_800BAF34;
    TimeHMS tm;
    f32 ys[3] = { 121.0f, 151.0f, 181.0f };
    UiElem *el = NULL;
    s32 a[4] = { 0x11366, 0x11364, 0x11366, 0x113C4 };
    s32 b[4] = { 0x113DA, 0x113D8, 0x113DC, 0x113DE };
    s32 i;
    s32 n;
    f32 x;
    f32 w;

    x = 299.0f;
    switch (mode) {
    case 2:
        Ui_AddElement(gUi[0], 0, D_80097250[0]);
        Ui_AddElement(gUi[0], 0, D_8009725C);
        strcpy(D_80123DB4[0], e.c);
        sprintf(D_80123DB4[1], D_800BAF68, D_80097CCC[0].score);
        strcpy(D_80123DB4[2], e.c);
        strcpy(D_80123DB4[3], e.c);
        strcpy(D_80123DB4[4], D_80097CCC[0].name);
        strcpy(D_80123DB4[5], e.c);
        w = Font_RightAlignX(D_80123DB4[1], 0x11200, x);
        Ui_SetElementPos(gUi[0], 0, D_80097250[0], w, ys[1]);
        break;
    case 3:
        n = 3;
        for (i = 10; i < n + 10; i++) {
            Ui_AddElement(gUi[0], 0, D_80097224[i]);
            Ui_AddElement(gUi[0], 0, D_80097224[i + 3]);
        }
        sprintf(D_80123DB4[0], D_800BAF68, D_80097CCC[1].score);
        sprintf(D_80123DB4[1], D_800BAF68, D_80097CCC[2].score);
        sprintf(D_80123DB4[2], D_800BAF68, D_80097CCC[3].score);
        for (i = 10; i < n + 10; i++) {
            w = Font_RightAlignX(D_80123DB4[i - 10], 0x11200, x);
            Ui_SetElementPos(gUi[0], 0, i + 0x17A, w, ys[i - 10]);
        }
        strcpy(D_80123DB4[3], D_80097CCC[1].name);
        strcpy(D_80123DB4[4], D_80097CCC[2].name);
        strcpy(D_80123DB4[5], D_80097CCC[3].name);
        break;
    case 0:
        Ui_AddElement(gUi[0], 0, D_80097250[0]);
        Ui_AddElement(gUi[0], 0, D_8009725C);
        sprintf(D_80123DB4[1], D_800BAF68, D_80097A28[cup].x20);
        strcpy(D_80123DB4[0], e.c);
        strcpy(D_80123DB4[2], e.c);
        strcpy(D_80123DB4[4], D_80097A28[cup].n0);
        strcpy(D_80123DB4[3], e.c);
        strcpy(D_80123DB4[5], e.c);
        w = Font_RightAlignX(D_80123DB4[1], 0x11200, x);
        Ui_SetElementPos(gUi[0], 0, D_80097250[0], w, ys[1]);
        break;
    case 1:
        n = 3;
        for (i = 10; i < n + 10; i++) {
            Ui_AddElement(gUi[0], 0, D_80097224[i]);
            Ui_AddElement(gUi[0], 0, D_80097224[i + 3]);
        }
        sprintf(D_80123DB4[0], D_800BAF68, D_80097A28[cup].x20);
        tm = Time_Split(D_80097A28[cup].x28);
        Time_Format(tm, buf.c, 8);
        strcpy(D_80123DB4[1], buf.c);
        tm = Time_Split(D_80097A28[cup].x30);
        Time_Format(tm, buf.c, 8);
        strcpy(D_80123DB4[2], buf.c);
        strcpy(D_80123DB4[3], D_80097A28[cup].n0);
        strcpy(D_80123DB4[4], D_80097A28[cup].n1);
        strcpy(D_80123DB4[5], D_80097A28[cup].n2);
        for (i = 10; i < n + 10; i++) {
            w = Font_RightAlignX(D_80123DB4[i - 10], 0x11200, x);
            Ui_SetElementPos(gUi[0], 0, i + 0x17A, w, ys[i - 10]);
        }
        break;
    }
    if (base != -1) {
        Res_Release(base + 0x112D0);
        Res_Release(base + 0x112DD);
        FIND_ELEM(base + 0x112D0);
        el->res = Res_Get(cup + 0x112D0);
        el->id = cup + 0x112D0;
        FIND_ELEM(base + 0x112DD);
        el->res = Res_Get(cup + 0x112DD);
        el->id = cup + 0x112DD;
        if (prev != mode && prev != -1) {
            Res_Release(a[prev]);
            FIND_ELEM(a[prev]);
            el->res = Res_Get(a[mode]);
            el->id = a[mode];
            Res_Release(b[prev]);
            FIND_ELEM(b[prev]);
            el->res = Res_Get(b[mode]);
            el->id = b[mode];
        }
    }
}
