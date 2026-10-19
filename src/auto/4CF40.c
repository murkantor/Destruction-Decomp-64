#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 pad8[8]; } Ent10;
typedef struct {
    u8 name[0x14];
    char a[10];
    char b[10];
    char c[10];
    s8 x32;
} Row;
typedef struct { char x0[0xB]; char xB[0xB]; } Name16;
typedef struct { s32 x0; u8 pad4[0x34]; } Record;
typedef struct { s32 x0; u8 pad4[0x14]; } Best18;
typedef struct { u8 pad0[0xC]; s32 xC; s32 x10; u8 pad14[0x54]; } Car68;

typedef struct { s32 v[12]; } S12;
extern S12 D_800BB1C0;
extern u8 D_800BB1B8;
typedef struct { char c[2]; } S2;
extern S2 D_800BB1BC;
extern s32 D_800972C0[];
extern void *gUi[];
extern s32 D_800958B4;
extern s32 D_80096038[];
extern s32 D_80097224[];
extern s32 D_800971DC[][3];
extern s32 D_800958A4;
extern s32 D_800958A8;
extern s32 D_80095D04[];
extern s32 D_80095D34[];
extern Ent10 D_80126188[];
extern Row D_801238F0[];
extern s32 D_80096720[];
extern s32 D_800958C0[];
extern Name16 D_80096510[];
extern char D_800BB1F0[];
extern char D_800BAF68[];
extern s32 D_80095A70;
extern Record D_80097A48[];
extern Best18 D_80130518[];
extern u8 D_80097335;
extern s32 D_80097D48;
extern u8 D_80097294[];
extern u8 D_800972A8[];
extern Car68 D_800958F8[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
char *strcpy(char *, const char *);
s32 func_8001B2F4(s32);
s32 func_8001B298(s32);
void func_80048B30(void *, s32, s32);
void Menu_LayoutResultRows(f32, void *, s32);
void Timer_Start(void *, u64);

static inline s32 Rank_Get(s32 i) {
    s32 j;
    s32 rank;
    s32 score;
    u8 flag;

    flag = 1;
    rank = -1;
    if (D_80126188[i].x4 >= 0) {
        score = D_80126188[i].x4;
        rank = 0;
        for (j = 0; j < 12; j++) {
            rank += (score < D_80126188[j].x4 && j != i);
        }
        while (flag) {
            if (D_801238F0[rank].name[0]) {
                rank++;
            } else {
                flag = 0;
            }
        }
    }
    return rank;
}

void func_8004C340(void) {
    S2 sp;
    S12 ids;
    char num[8];
    char empty[8];
    s32 i;
    s32 j;
    s32 rank;
    s32 score;
    u8 flag;
    s32 p;
    s32 k;
    s32 n;
    s32 q;

    empty[0] = D_800BB1B8;
    sp = D_800BB1BC;
    ids = D_800BB1C0;
    Ui_ResetScreen(0x1132C, 0x1132D);
    for (i = 0; i < 12; i++) {
        D_800972C0[i] = ids.v[i];
    }
    Ui_AddElement(gUi[0], 2, 0x7A);
    Ui_AddElement(gUi[0], 2, 0xBD);
    for (i = 0; (D_80096038[D_800958B4] < 7) ? i < D_80096038[D_800958B4] : i < 6; i++) {
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
        D_80126188[i].x4 = D_80095D04[i] + D_80095D34[i];
    }
    for (n = 11 * 0x33; n >= 0; n -= 0x33) {
        ((u8 *)D_801238F0)[n] = 0;
    }
    for (i = 0; i < 12; i++) {
        rank = Rank_Get(i);
        if (rank < 0) {
            continue;
        }
        if (D_800958A8 == 0) {
            D_80126188[i].x0 = D_80096720[rank];
        }
        p = func_8001B2F4(i);
        for (k = 0; k < D_800958A4; k++) {
            if (p == D_800958C0[k]) {
                break;
            }
        }
        if (k != D_800958A4) {
            D_801238F0[rank].x32 = k;
        } else {
            D_801238F0[rank].x32 = -1;
        }
        q = func_8001B2F4(i);
        if (D_800958A8 == 3 && D_801238F0[rank].x32 != -1) {
            sprintf((char *)D_801238F0[rank].name, D_800BB1F0);
            sprintf(num, D_800BAF68, D_801238F0[rank].x32 + 1);
            strcat((char *)D_801238F0[rank].name, num);
        } else {
            sprintf((char *)D_801238F0[rank].name, D_80096510[q].x0);
            strcat((char *)D_801238F0[rank].name, sp.c);
            strcat((char *)D_801238F0[rank].name, D_80096510[q].xB);
        }
        sprintf(D_801238F0[rank].a, D_800BAF68, D_80095D04[i]);
        sprintf(D_801238F0[rank].b, D_800BAF68, D_80095D34[i]);
        sprintf(D_801238F0[rank].c, D_800BAF68, D_80126188[i].x4);
    }
    for (i = D_80095A70; i < 12; i++) {
        strcpy((char *)D_801238F0[i].name, empty);
        strcpy(D_801238F0[i].a, empty);
        strcpy(D_801238F0[i].b, empty);
        strcpy(D_801238F0[i].c, empty);
        D_801238F0[i].x32 = -1;
    }
    if (D_800958A4 == 1) {
        j = func_8001B298(D_800958C0[0]);
        if (D_80126188[j].x4 > D_80097A48[D_800958B4].x0 && D_801238F0[0].x32 != -1) {
            D_80130518[D_800958B4].x0 = D_80126188[j].x4;
            D_80097335 = 1;
            D_80097D48 = 0;
        }
    }
    func_80048B30(D_80097294, 3, 1);
    Menu_LayoutResultRows(26.0f, D_800972A8, 1);
    for (i = 0; i < 4; i++) {
        D_800958F8[i].xC = 2;
        D_800958F8[i].x10 = 0;
        Timer_Start(&D_800958F8[i], 1000000000);
    }
}
