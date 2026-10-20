#include "common.h"
#include "m2c.h"

typedef struct { f32 v[3]; } F3;
typedef struct { s32 x0; s32 x4; s32 x8; u8 padC[8]; } Thr;
typedef struct { s32 v[3]; } Rec3;
typedef struct { u8 pad0[8]; u8 x8[0xC]; s32 x14; s32 x18; u8 pad1C[0x4C]; } Car;

extern F3 D_800BB2A0;
extern s32 D_80095D04[];
extern s32 D_80095D34[];
extern s32 D_800958A4;
extern char gSaveSlotText[][0x32];
extern Thr D_80097CEC[];
extern Rec3 D_80097D18[];
extern u8 D_80097335;
extern s32 D_80097D48;
extern s32 D_800971DC[][3];
extern char D_80130138[][3][9];
extern char D_80123C88[][0x32];
extern void *gUi[];
extern Car D_800958F0[];
extern char D_800BAF68[];
void Ui_ResetScreen(s32, s32);
s32 sprintf(char *, const char *, ...);
f32 Font_RightAlignX(char *, s32, f32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void Ui_SetElementColor(s32, s32);
void Msg_Queue(s32, s32, s32, s32);
void Timer_Start(void *, u64);

void Results_DrawTeamScores(void) {
    F3 xs = D_800BB2A0;
    f32 ys[7] = { 92.0f, 111.0f, 155.0f, 175.0f, 129.0f, 191.0f, 0.0f };
    s32 best[3] = { 0, 0, 0 };
    s32 idx[3] = { -1, -1, -1 };
    s32 sc[4][3];
    s32 i;
    s32 j;
    s32 minIdx;
    f32 w;

    minIdx = 5;
    Ui_ResetScreen(0x112FA, 0x112FB);
    for (i = 0; i < 4; i++) {
        s32 a = D_80095D04[i];
        s32 b = D_80095D34[i];
        sc[i][0] = a;
        sc[i][1] = b;
        sc[i][2] = a * 10 + b * 5;
    }
    if (D_800958A4 == 2) {
        sprintf(gSaveSlotText[0], "PLAYER 1");
        sprintf(gSaveSlotText[1], "AI TEAM");
        sprintf(gSaveSlotText[2], "PLAYER 2");
        sprintf(gSaveSlotText[3], "AI TEAM");
        if (sc[0][0] > sc[2][0]) {
            best[0] = sc[0][0];
            idx[0] = 0;
        } else {
            best[0] = sc[2][0];
            idx[0] = 1;
        }
        if (sc[0][1] > sc[2][1]) {
            best[1] = sc[0][1];
            idx[1] = 0;
        } else {
            best[1] = sc[2][1];
            idx[1] = 1;
        }
        if (sc[0][2] > sc[2][2]) {
            best[2] = sc[0][2];
            idx[2] = 0;
        } else {
            best[2] = sc[2][2];
            idx[2] = 1;
        }
    } else {
        sprintf(gSaveSlotText[0], "PLAYER 1");
        sprintf(gSaveSlotText[1], "PLAYER 2");
        sprintf(gSaveSlotText[2], "PLAYER 3");
        sprintf(gSaveSlotText[3], "PLAYER 4");
        for (i = 0; i < 4; i++) {
            if (best[0] < sc[i][0]) {
                best[0] = sc[i][0];
                idx[0] = i;
            }
            if (best[1] < sc[i][1]) {
                best[1] = sc[i][1];
                idx[1] = i;
            }
            if (best[2] < sc[i][2]) {
                best[2] = sc[i][2];
                idx[2] = i;
            }
        }
    }
    if (best[0] > D_80097CEC[0].x0) {
        D_80097D18[idx[0]].v[0] = best[0];
        D_80097335 = 1;
        if (idx[0] < minIdx) {
            minIdx = idx[0];
        }
    }
    if (best[1] > D_80097CEC[1].x0) {
        D_80097D18[idx[1]].v[1] = best[1];
        D_80097335 = 1;
        if (idx[1] < minIdx) {
            minIdx = idx[1];
        }
    }
    if (best[2] > D_80097CEC[2].x0) {
        D_80097D18[idx[2]].v[2] = best[2];
        D_80097335 = 1;
        if (idx[2] < minIdx) {
            minIdx = idx[2];
        }
    }
    if (D_80097335) {
        D_80097D48 = minIdx;
    }
    for (i = 0; i < 4; i++) {
        sprintf(D_80130138[i][0], D_800BAF68, sc[i][0]);
        sprintf(D_80130138[i][1], D_800BAF68, sc[i][1]);
        sprintf(D_80130138[i][2], D_800BAF68, sc[i][2]);
        for (j = 0; j < 3; j++) {
            w = Font_RightAlignX(D_80130138[i][j], 0x111F6, xs.v[j]);
            Ui_SetElementPos(gUi[0], 0, D_800971DC[i][j], w, ys[i]);
        }
    }
    i = sc[0][2] + sc[1][2];
    j = sc[2][2] + sc[3][2];
    sprintf(D_80123C88[0], D_800BAF68, i);
    Ui_SetElementPos(gUi[0], 0, 0x17E, Font_RightAlignX(D_80123C88[0], 0x111F6, xs.v[2]), ys[4]);
    sprintf(D_80123C88[1], D_800BAF68, j);
    Ui_SetElementPos(gUi[0], 0, 0x17F, Font_RightAlignX(D_80123C88[1], 0x111F6, xs.v[2]), ys[5]);
    if (j < i) {
        Ui_SetElementColor(0x17E, 0x111FC);
        Msg_Queue(0x83, 0, 1, 0);
    } else if (i < j) {
        Ui_SetElementColor(0x17F, 0x111FC);
        Msg_Queue(0x82, 0, 1, 0);
    }
    for (i = 0; i < 4; i++) {
        D_800958F0[i].x14 = 2;
        D_800958F0[i].x18 = 0;
        Timer_Start(D_800958F0[i].x8, 1000000000);
    }
}
