#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { char c[8]; } Str8;
typedef struct { s32 v[12]; } Ids;
typedef struct { u8 pad[0xB4]; u8 xB4; } Model;
typedef struct { Model *x0; u8 pad[0x64]; } PlayerSlot;
typedef struct { s32 track; u8 pad[0x24]; PlayerSlot pl[4]; } Setup;
typedef struct { char name[0xC]; s32 score; u8 pad[4]; } HiScore;
typedef struct { u8 pad[9]; char nameB[9]; char nameA[0x16]; u64 timeB; u64 timeA; } Record;
typedef struct { u8 pad[8]; s32 x8; u8 padC[4]; } Prize;
typedef struct { u8 pad[0x34]; s32 x34; } Car;
typedef struct { u8 pad[0x10]; s32 x10; } Race;
typedef struct { u8 pad[0x1058]; u8 x1058[0x1100 - 0x1058]; Race *x1100; } Game;
extern Game *gGame;
extern void *gUi[];
extern Str8 D_800BA7C0;
extern Ids D_800BA7C8;
extern s32 D_800958B4;
#define gPlayers ((PlayerSlot *)((u8 *)&D_800958B4 + 0x28))
extern s8 D_800D31CC;
extern u8 D_800D1BAC;
extern s32 D_800958A8[];
extern u32 D_800958AC;
extern s32 D_800958BC;
extern s32 D_800958B0;
extern char gSaveSlotText[][0x32];
extern HiScore D_80097CCC[];
extern Record D_80097A28[];
extern f32 D_800D1BA0;
extern f32 D_800D1BA4;
extern Car *D_800958F0;
extern s32 D_80095CB8[];
extern Prize *D_80095CA8[];
extern s32 D_801252F4;
extern s32 D_80095CD4[];
extern s32 D_80096724[];
extern s32 D_80097360;
extern char D_80123BF2[];
extern char D_80123D1E[];
extern u8 D_80096ED0[];
extern s32 D_80096EB8;
extern u8 D_80097354;
extern s32 D_80097358;
extern char D_800BA7F8[], D_800BA804[], D_800BA814[], D_800BA834[], D_800BA85C[], D_800BA87C[], D_800BA880[];
extern char D_800BA884[], D_800BA890[], D_800BA8B4[], D_800BA8CC[], D_800BA8EC[], D_800BA914[], D_800BA928[];
extern char D_800BA938[], D_800BA954[], D_800BA96C[], D_800BA98C[], D_800BA99C[], D_800BA9A8[], D_800BA9B8[];
extern char D_800BA9D0[], D_800BA9E4[], D_800BA9F8[], D_800BAA14[], D_800BAA20[], D_800BAA34[], D_800BAA48[];
extern char D_800BAA5C[];
void Rumble_DisallowRetry(void);
void func_80062FF0(Model *);
void func_80044B44(s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
char *strcpy(char *, const char *);
char *strncat(char *, const char *, u32);
f32 Time_ToSeconds(void *);
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
s32 func_8001B298(s32);
void Msg_Queue(s32, s32, s32, s32);

void Results_Setup(void) {
    TimeHMS ts;
    Str8 buf = D_800BA7C0;
    Ids msgs = D_800BA7C8;
    s32 track;
    s32 n;
    s32 v;
    s32 diff;
    s32 i;
    Setup *st;

    Rumble_DisallowRetry();
    st = (Setup *)&D_800958B4;
    track = st->track;
    for (i = 0; i < D_800D31CC; i++) {
        gPlayers[i].x0->xB4 = 6;
        func_80062FF0(gPlayers[i].x0);
    }
    func_80044B44(track);
    Ui_AddElement(gUi[0], 2, 0xDF);
    Ui_AddElement(gUi[0], 2, 0xDE);
    D_800D1BAC = 0;
    if (D_800958A8[0] == 3) {
        switch (D_800958AC) {
        case 1:
            Ui_AddElement(gUi[0], 0, 0x17A);
            if (D_800958BC == 0) {
                Ui_AddElement(gUi[0], 0, 0x17B);
                Ui_AddElement(gUi[0], 0, 0x17D);
                Ui_AddElement(gUi[0], 0, 0x17E);
                Ui_AddElement(gUi[0], 0, 0x17F);
                Ui_AddElement(gUi[0], 0, 0x181);
                sprintf(gSaveSlotText[0], D_800BA7F8);
                sprintf(gSaveSlotText[1], D_800BA804);
                sprintf(gSaveSlotText[3], D_800BA814);
                sprintf(gSaveSlotText[4], D_800BA834);
                sprintf(gSaveSlotText[5], D_800BA85C);
                sprintf(gSaveSlotText[7], D_800BA87C, D_80097CCC[0].score);
                strcat(gSaveSlotText[7], D_800BA880);
                strncat(gSaveSlotText[7], D_80097CCC[0].name, 8);
            } else {
                sprintf(gSaveSlotText[0], D_800BA884, D_800958BC + 1);
            }
            break;
        case 3:
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17D);
            Ui_AddElement(gUi[0], 0, 0x17E);
            sprintf(gSaveSlotText[0], D_800BA7F8);
            sprintf(gSaveSlotText[3], D_800BA890);
            sprintf(gSaveSlotText[4], D_800BA8B4);
            Ui_AddElement(gUi[0], 0, 0x17F);
            Ui_AddElement(gUi[0], 0, 0x180);
            sprintf(gSaveSlotText[5], D_800BA8CC);
            sprintf(gSaveSlotText[6], D_800BA8EC);
            break;
        case 0:
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17D);
            Ui_AddElement(gUi[0], 0, 0x17E);
            sprintf(gSaveSlotText[0], D_800BA7F8);
            sprintf(gSaveSlotText[3], D_800BA890);
            sprintf(gSaveSlotText[4], D_800BA8B4);
            break;
        case 2:
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17B);
            Ui_AddElement(gUi[0], 0, 0x17C);
            Ui_AddElement(gUi[0], 0, 0x17D);
            Ui_AddElement(gUi[0], 0, 0x17E);
            Ui_AddElement(gUi[0], 0, 0x17F);
            Ui_AddElement(gUi[0], 0, 0x181);
            Ui_AddElement(gUi[0], 0, 0x182);
            sprintf(gSaveSlotText[0], D_800BA7F8);
            sprintf(gSaveSlotText[1], D_800BA914);
            sprintf(gSaveSlotText[2], D_800BA928);
            sprintf(gSaveSlotText[3], D_800BA938);
            sprintf(gSaveSlotText[4], D_800BA954);
            sprintf(gSaveSlotText[5], D_800BA96C);
            sprintf(gSaveSlotText[7], D_800BA87C, D_80097CCC[1].score);
            strcat(gSaveSlotText[7], D_800BA880);
            strncat(gSaveSlotText[7], D_80097CCC[1].name, 8);
            sprintf(gSaveSlotText[8], D_800BA87C, D_80097CCC[2].score);
            strcat(gSaveSlotText[8], D_800BA880);
            strncat(gSaveSlotText[8], D_80097CCC[2].name, 8);
            break;
        }
    } else {
        D_800D1BA4 = D_800D1BA0 = Time_ToSeconds(gGame->x1058);
        if (D_800958A8[0] == 2) {
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17B);
            Ui_AddElement(gUi[0], 0, 0x17C);
            Ui_AddElement(gUi[0], 0, 0x17D);
            sprintf(gSaveSlotText[0], D_800BA98C);
            sprintf(gSaveSlotText[1], D_800BA99C);
            ts = Time_Split(D_80097A28[D_800958B4].timeA);
            Time_Format(ts, buf.c, 8);
            strcpy(gSaveSlotText[2], buf.c);
            strcat(gSaveSlotText[2], D_800BA880);
            strncat(gSaveSlotText[2], D_80097A28[D_800958B4].nameA, 8);
            ts = Time_Split(D_80097A28[D_800958B4].timeB);
            Time_Format(ts, buf.c, 8);
            strcpy(gSaveSlotText[3], buf.c);
            strcat(gSaveSlotText[3], D_800BA880);
            strncat(gSaveSlotText[3], D_80097A28[D_800958B4].nameB, 8);
        }
        if (D_800958A8[0] == 1) {
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17D);
            Ui_AddElement(gUi[0], 0, 0x17E);
            sprintf(gSaveSlotText[0], D_800BA7F8);
            sprintf(gSaveSlotText[3], D_800BA890);
            sprintf(gSaveSlotText[4], D_800BA8B4);
            if (gGame->x1100->x10 == 0) {
                Ui_AddElement(gUi[0], 0, 0x17F);
                Ui_AddElement(gUi[0], 0, 0x180);
                sprintf(gSaveSlotText[5], D_800BA8CC);
                sprintf(gSaveSlotText[6], D_800BA8EC);
            }
        }
        if (D_800958A8[0] == 0) {
            Ui_AddElement(gUi[0], 0, 0x17A);
            Ui_AddElement(gUi[0], 0, 0x17B);
            Ui_AddElement(gUi[0], 0, 0x17D);
            Ui_AddElement(gUi[0], 0, 0x17E);
            Ui_AddElement(gUi[0], 0, 0x181);
            sprintf(gSaveSlotText[0], D_800BA7F8);
            sprintf(gSaveSlotText[1], D_800BA9A8);
            sprintf(gSaveSlotText[3], D_800BA890);
            sprintf(gSaveSlotText[4], D_800BA8B4);
            if (gGame->x1100->x10 == 0) {
                Ui_AddElement(gUi[0], 0, 0x17F);
                Ui_AddElement(gUi[0], 0, 0x180);
                sprintf(gSaveSlotText[5], D_800BA8CC);
                sprintf(gSaveSlotText[6], D_800BA8EC);
            }
            n = func_8001B298(D_800958F0->x34);
            if (D_801252F4 == D_80095CB8[D_800958B0] - 1) {
                sprintf(gSaveSlotText[7], D_800BA9B8);
            } else {
                Ui_AddElement(gUi[0], 0, 0x182);
                v = D_80095CA8[D_800958B0][D_801252F4].x8;
                if (v == 1) {
                    sprintf(gSaveSlotText[8], D_800BA9D0, 1);
                } else {
                    sprintf(gSaveSlotText[8], D_800BA9E4, v);
                }
                diff = D_80095CA8[D_800958B0][D_801252F4].x8 - D_80095CD4[n];
                if (diff <= 0) {
                    sprintf(D_80123BF2, D_800BA9A8);
                    sprintf(D_80123BF2 + 0x12C, D_800BA9F8);
                    D_80097360 = 12;
                } else {
                    for (i = 1; i < 9; i++) {
                        if (D_80096724[i - 1] < diff) {
                            break;
                        }
                    }
                    sprintf(D_80123D1E, D_800BAA14, i);
                    switch (i) {
                    case 1:
                        strcat(D_80123D1E, D_800BAA20);
                        break;
                    case 2:
                        strcat(D_80123D1E, D_800BAA34);
                        break;
                    case 3:
                        strcat(D_80123D1E, D_800BAA48);
                        break;
                    default:
                        strcat(D_80123D1E, D_800BAA5C);
                        break;
                    }
                    D_80097360 = i;
                }
            }
        }
    }
    Msg_Queue(msgs.v[track], 0x1F4, 1, 0);
    for (i = 0; i < 12; i++) {
        if (i < 4) {
            D_80096ED0[i] = 1;
        } else {
            D_80096ED0[i] = 0;
        }
    }
    D_80096EB8 = 0;
    D_80097354 = 1;
    D_80097358 = 0;
}
