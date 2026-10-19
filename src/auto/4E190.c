#include "common.h"
#include "m2c.h"

typedef struct { u8 x0[0x14]; char x14[0xA]; char x1E[0xA]; char x28[0xA]; s8 x32; } Row;
typedef struct { char first[0xB]; char last[0xB]; } Name;
typedef struct { s32 x0; u8 pad[0xC]; } Score;
typedef struct { u8 pad[0xC]; s32 xC; s32 x10; u8 pad14[0x68 - 0x14]; } Timer68;
typedef struct { s32 v[12]; } Ids;
extern void *gUi[];
typedef struct { char c[2]; } Str2;
typedef struct { char c[1]; } Str1;
extern Str2 D_800BB1BC;
extern Str1 D_800BB1B8;
extern Ids D_800BB238;
extern s32 D_800972C0[];
extern s32 D_800958B4;
extern s32 D_80096038[];
extern s32 D_80097224[];
extern s32 D_800971DC[][3];
extern Row D_801238F0[];
extern Score D_80126188[];
extern s32 D_80095CD4[];
extern Name D_80096510[];
typedef struct { s32 count; u8 pad[0x18]; s32 list[4]; } Players;
extern Players D_800958A4;
extern char D_800BAF68[];
extern Timer68 D_800958F8[];
extern u8 D_800972A0[];
extern u8 D_800972A8[];
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 func_8001B2F4(s32);
s32 sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
char *strcpy(char *, const char *);
void HiScore_DrawTable(void *, s32, s32);
void Menu_LayoutResultRows(f32, void *, s32);
void Timer_Start(void *, s64);

void func_8004D590(void) {
    Str2 sfx;
    Ids ids;
    Str1 empty;
    s32 i;
    s32 j;
    s32 k;
    s32 rank;
    s32 total;
    s32 n;
    u8 busy;
    s32 m;
    s32 l;

    sfx = D_800BB1BC;
    empty = D_800BB1B8;
    ids = D_800BB238;
    Ui_ResetScreen(0x112EA, 0x112EB);
    for (i = 0; i < 12; i++) {
        D_800972C0[i] = ids.v[i];
    }
    Ui_AddElement(gUi[0], 2, 0x41);
    Ui_AddElement(gUi[0], 2, 0xC0);
    for (j = 0; (D_80096038[D_800958B4] < 7) ? (j < D_80096038[D_800958B4]) : (j < 6); j++) {
        Ui_AddElement(gUi[0], 0, D_80097224[j]);
        for (l = 0; l < 2; l++) {
            Ui_AddElement(gUi[0], 0, D_800971DC[j][l]);
        }
    }
    for (i = 0; i < 12; i++) {
        Ui_AddElement(gUi[0], 2, D_800972C0[i]);
    }
    Ui_AddElement(gUi[0], 2, 0xC1);
    for (m = 11 * sizeof(Row); m >= 0; m -= sizeof(Row)) {
        ((char *)D_801238F0)[m] = 0;
    }
    for (i = 0; i < 12; i++) {
        rank = 0;
        total = D_80126188[i].x0 + D_80095CD4[i];
        for (k = 0; k < 12; k++) {
            rank += (total < D_80126188[k].x0 + D_80095CD4[k]) & (k != i);
        }
        busy = 1;
        do {
            if (D_801238F0[rank].x0[0] != 0) {
                rank++;
            } else {
                busy = 0;
            }
        } while (busy);
        n = func_8001B2F4(i);
        sprintf((char *)D_801238F0[rank].x0, D_80096510[n].first);
        strcat((char *)D_801238F0[rank].x0, sfx.c);
        strcat((char *)D_801238F0[rank].x0, D_80096510[n].last);
        sprintf(D_801238F0[rank].x14, D_800BAF68, D_80126188[i].x0);
        sprintf(D_801238F0[rank].x1E, D_800BAF68, D_80095CD4[i] + D_80126188[i].x0);
        n = func_8001B2F4(i);
        for (k = 0; k < D_800958A4.count; k++) {
            if (n == D_800958A4.list[k]) {
                break;
            }
        }
        if (k != D_800958A4.count) {
            D_801238F0[rank].x32 = k;
        } else {
            D_801238F0[rank].x32 = -1;
        }
    }
    for (i = D_80096038[D_800958B4]; i < 12; i++) {
        strcpy((char *)D_801238F0[i].x0, empty.c);
        strcpy(D_801238F0[i].x14, empty.c);
        strcpy(D_801238F0[i].x1E, empty.c);
        strcpy(D_801238F0[i].x28, empty.c);
        D_801238F0[i].x32 = -1;
    }
    HiScore_DrawTable(D_800972A0, 2, 1);
    Menu_LayoutResultRows(25.0f, D_800972A8, 1);
    for (i = 0; i < 4; i++) {
        D_800958F8[i].xC = 2;
        D_800958F8[i].x10 = 0;
        Timer_Start(&D_800958F8[i], 1000000000);
    }
}
