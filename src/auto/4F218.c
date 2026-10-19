#include "common.h"
#include "m2c.h"

typedef struct { s32 x0[18]; } UiElemDesc;
typedef struct {
    char name[0x10];
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
} Standing;
typedef struct { s32 a; s32 b; s32 c; } Score3;
typedef struct { s32 count; s32 pad4[6]; s32 ids[8]; } PlayerList;
typedef struct { u8 pad0[0xC]; s32 xC; s32 x10; u8 pad14[0x54]; } MenuT;

extern void *gUi[];
extern UiElemDesc D_800BB2BC;
extern UiElemDesc D_800BB304;
extern UiElemDesc D_800BB34C;
extern s32 D_800958BC[];
extern PlayerList D_800958A4;
extern Standing D_80131D88[];
extern s32 D_80095D04[];
extern Score3 D_80095D94[];
extern char D_800BAF80[];
extern char D_800BB394[];
extern char D_800BB3A4[];
extern s32 D_80095A70[];
extern s32 D_800972F4[];
extern s32 D_80097CD8;
extern s32 D_80125C5C;
extern u8 D_80097335;
extern s32 D_80097D48;
extern MenuT D_800958F8[];
extern char D_80123C88[];

void Ui_ResetScreen(s32, s32);
void Ui_AddSpriteElement(void *, UiElemDesc *);
void Ui_AddElement(void *, s32, s32);
s32 func_8001B2F4(s32);
int sprintf(char *, const char *, ...);
void func_8004E144(s32, s32);
void Timer_Start(void *, u64);

void Results_Init(void)
{
    UiElemDesc a = D_800BB2BC;
    UiElemDesc b = D_800BB304;
    UiElemDesc c = D_800BB34C;
    s32 i;
    s32 j;
    s32 cpu;
    s32 r;
    s32 total;
    s32 rest;
    s32 rank;
    s32 score;
    u8 searching;
    s32 mode;

    Ui_ResetScreen(0x112F8, 0x112F9);
    switch (D_800958BC[0]) {
    case 1:
        Ui_AddSpriteElement(gUi[0], &b);
        Ui_AddElement(gUi[0], 2, 0xD5);
        break;
    case 2:
        Ui_AddSpriteElement(gUi[0], &c);
        Ui_AddElement(gUi[0], 2, 0xD6);
        break;
    case 3:
        Ui_AddSpriteElement(gUi[0], &a);
        Ui_AddElement(gUi[0], 2, 0xD1);
        break;
    }

    cpu = D_800958A4.count + 1;
    for (i = 0; i < 12; i++) {
        total = D_80095D04[i];
        rest = total - D_80095D94[i].a - D_80095D94[i].b - D_80095D94[i].c;
        D_80131D88[i].x10 = total - rest;
        D_80131D88[i].x14 = rest;
        D_80131D88[i].x18 = total;
        r = func_8001B2F4(i);
        for (j = 0; j < D_800958A4.count; j++) {
            if (r == D_800958A4.ids[j])
                break;
        }
        if (j != D_800958A4.count) {
            D_80131D88[i].x1C = j;
            sprintf(D_80131D88[i].name, D_800BAF80, j + 1);
        } else {
            D_80131D88[i].x1C = -1;
            sprintf(D_80131D88[i].name, D_800BB394, cpu++);
        }
    }

    for (i = 0; i < D_80095A70[0]; i++) {
        if (D_80131D88[i].x18 > 0) {
            rank = 0;
            j = 0;
            score = D_80131D88[i].x18;
            for (; j < D_80095A70[0]; j++) {
                rank += (score < D_80131D88[j].x18) & (j != i);
            }
            searching = 1;
            while (searching) {
                if (D_800972F4[rank] != -1) {
                    rank++;
                } else {
                    searching = 0;
                    D_800972F4[rank] = i;
                }
            }
        }
    }

    mode = 0;
    if (D_80131D88[D_800972F4[0]].x1C != -1 || D_80131D88[D_800972F4[1]].x1C != -1 ||
        D_80131D88[D_800972F4[2]].x1C != -1 || D_80131D88[D_800972F4[3]].x1C != -1) {
        func_8004E144(-1, 0);
    } else {
        mode = 4;
        func_8004E144(-1, 4);
    }

    if (D_80131D88[D_800972F4[0]].x1C != -1 && D_800958BC[0] == 3 &&
        D_80131D88[D_800972F4[0]].x18 > D_80097CD8) {
        D_80125C5C = D_80131D88[D_800972F4[0]].x18;
        D_80097335 = 1;
        D_80097D48 = D_800972F4[0];
    }

    for (i = 0; i < 4; i++) {
        D_800958F8[i].xC = 2;
        D_800958F8[i].x10 = mode;
        Timer_Start(&D_800958F8[i], 2000000000);
    }

    if (D_800958BC[0] != 3) {
        Ui_AddElement(gUi[0], 0, 0x17E);
        sprintf(D_80123C88, D_800BB3A4, D_800958BC[0] + 1);
    }
}
