#include "common.h"
#include "m2c.h"

typedef struct {
    u8 name[0x14];
    char x14[0xA];
    char x1E[0xA];
    char x28[0xA];
    s8 x32;
} Entry;
typedef struct { s32 x0; s32 x4; u8 pad8[8]; } Score;
typedef struct { s32 x0; u8 pad4[0x34]; } Record;
typedef struct { s32 x0; u8 pad4[0x14]; } Slot;
typedef struct { u8 pad0[0xC]; s32 xC; s32 x10; u8 pad14[0x54]; } Timer68;
typedef struct { char s[1]; } Str1;
typedef struct { char s[2]; } Str2;
typedef struct { s32 v[12]; } Ids;

extern Str1 D_800BB1B8;
extern Str2 D_800BB1BC;
extern Ids D_800BB1F8;
extern char D_800BB1F0[];
extern char D_800BAF68[];
extern void *gUi[];
extern s32 D_800972C0[];
extern s32 D_800958A4;
extern s32 D_800958A8;
extern s32 D_800958B4;
extern s32 D_800958C0[];
extern s32 D_80096038[];
extern s32 D_80097224[];
extern s32 D_800971DC[][3];
extern s32 D_80095D04[];
extern s32 D_80095D64[];
extern Score D_80126188[];
extern Entry D_801238F0[];
extern s32 D_80096720[];
extern char D_80096510[][0x16];
extern s32 D_80095A70;
extern Record D_80097A48[];
extern Slot D_80130518[];
extern u8 D_80097335;
extern s32 D_80097D48;
extern u8 D_80097294[];
extern f32 D_800972A8[];
extern Timer68 D_800958F8[];

void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 func_8001B2F4(s32);
s32 func_8001B298(s32);
s32 sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
char *strcpy(char *, const char *);
void func_80048B30(void *, s32, s32);
void Menu_LayoutResultRows(f32, f32 *, s32);
void Timer_Start(void *, u64);

static inline s32 getRank(s32 i) {
    s32 j;
    s32 rank;
    u8 found;

    found = 1;
    rank = -1;
    if (D_80126188[i].x4 >= 0) {
        s32 sc = D_80126188[i].x4;
        rank = 0;
        for (j = 0; j < 12; j++) {
            rank += (sc < D_80126188[j].x4) & (j != i);
        }
        while (found) {
            if (D_801238F0[rank].name[0] != 0) {
                rank++;
            } else {
                found = 0;
            }
        }
    }
    return rank;
}

void Results_BuildStandings(void) {
    Str2 space;
    Ids ids;
    char tmp[8];
    Str1 empty;
    s32 i;
    s32 j;
    s32 rank;
    s32 p;
    s32 n;
    s32 q;

    empty = D_800BB1B8;
    space = D_800BB1BC;
    ids = D_800BB1F8;
    Ui_ResetScreen(0x1132E, 0x1132F);
    for (i = 0; i < 12; i++) {
        D_800972C0[i] = ids.v[i];
    }
    Ui_AddElement(gUi[0], 2, 0xBF);
    Ui_AddElement(gUi[0], 2, 0xBE);
    for (i = 0; (D_80096038[D_800958B4] < 7) ? (i < D_80096038[D_800958B4]) : (i < 6); i++) {
        Ui_AddElement(gUi[0], 0, D_80097224[i]);
        for (j = 0; j < 3; j++) {
            Ui_AddElement(gUi[0], 0, D_800971DC[i][j]);
        }
    }
    for (i = 0; i < 12; i++) {
        Ui_AddElement(gUi[0], 2, D_800972C0[i]);
    }
    for (i = 0; i < D_800958A4; i++) {
        Ui_AddElement(gUi[0], 2, i + 0xC1);
    }
    for (i = 0; i < 12; i++) {
        D_80126188[i].x4 = D_80095D04[i] + D_80095D64[i];
    }
    {
        s32 k;
        for (k = 11 * 0x33; k >= 0; k -= 0x33) {
            ((u8 *)D_801238F0)[k] = 0;
        }
    }
    for (i = 0; i < 12; i++) {
        rank = getRank(i);
        if (rank >= 0) {
            if (D_800958A8 == 0) {
                D_80126188[i].x0 = D_80096720[rank];
            }
            p = func_8001B2F4(i);
            for (n = 0; n < D_800958A4; n++) {
                if (p == D_800958C0[n]) {
                    break;
                }
            }
            if (n != D_800958A4) {
                D_801238F0[rank].x32 = n;
            } else {
                D_801238F0[rank].x32 = -1;
            }
            q = func_8001B2F4(i);
            if (D_800958A8 == 3 && D_801238F0[rank].x32 != -1) {
                sprintf((char *)D_801238F0[rank].name, D_800BB1F0);
                sprintf(tmp, D_800BAF68, D_801238F0[rank].x32 + 1);
                strcat((char *)D_801238F0[rank].name, tmp);
            } else {
                sprintf((char *)D_801238F0[rank].name, D_80096510[q]);
                strcat((char *)D_801238F0[rank].name, space.s);
                strcat((char *)D_801238F0[rank].name, &D_80096510[q][11]);
            }
            sprintf(D_801238F0[rank].x14, D_800BAF68, D_80095D64[i]);
            sprintf(D_801238F0[rank].x1E, D_800BAF68, D_80095D04[i]);
            sprintf(D_801238F0[rank].x28, D_800BAF68, D_80126188[i].x4);
        }
    }
    for (i = D_80095A70; i < 12; i++) {
        strcpy((char *)D_801238F0[i].name, empty.s);
        strcpy(D_801238F0[i].x14, empty.s);
        strcpy(D_801238F0[i].x1E, empty.s);
        strcpy(D_801238F0[i].x28, empty.s);
        D_801238F0[i].x32 = -1;
    }
    if (D_800958A4 == 1) {
        i = func_8001B298(D_800958C0[0]);
        if (D_80126188[i].x4 > D_80097A48[D_800958B4].x0 && D_801238F0[0].x32 != -1) {
            D_80130518[D_800958B4].x0 = D_80126188[i].x4;
            D_80097335 = 1;
            D_80097D48 = 0;
        }
    }
    func_80048B30(D_80097294, 3, 1);
    Menu_LayoutResultRows(25.0f, D_800972A8, 1);
    for (i = 0; i < 4; i++) {
        D_800958F8[i].xC = 2;
        D_800958F8[i].x10 = 0;
        Timer_Start(&D_800958F8[i], 1000000000);
    }
}
