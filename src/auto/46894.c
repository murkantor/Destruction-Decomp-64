#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x14]; s32 x14; s32 x18; u8 pad1C[0x2C]; } UiDesc;
typedef struct { s32 v[4]; } S4;
typedef struct { s32 v[12]; } S12;
typedef struct { s32 v[3]; } S3;
typedef struct { char s[13]; } Str13;
typedef struct { char s[12]; } Str12;
typedef struct { char s[28]; } Str28;
typedef struct { u8 pad0[0xB4]; u8 xB4; } CarCfg;
typedef struct { u8 pad0[0xC]; CarCfg *xC; } CarInfo;
typedef struct { u8 pad0[0x10]; s32 x10; } CarStat;
typedef struct { u8 pad0[0x38]; CarStat *x38; CarInfo *x3C; } Car;
typedef struct { u8 pad0[4]; s32 x4; u8 pad8[8]; s32 x10; u8 pad14[0x60]; Car *cars[12]; } Race;
typedef struct { u8 pad0[0x1058]; u64 x1058; u8 pad1060[0xA0]; Race *race; } Game;
typedef struct { u8 pad0[0xD]; u8 xD; u8 padE[0x26]; s32 x34; } PlayerInfo;
typedef struct { PlayerInfo *x0; u8 pad4[0x64]; } Player;
typedef struct { s32 x0; s32 x4; u64 x8; u64 x10; } Slot;

extern UiDesc D_800BAC14;
extern UiDesc D_800BAB50;
extern S4 D_800BAC5C;
extern S12 D_800BABA8;
extern S12 D_800BABD8;
extern S3 D_800BAC08;
extern Str13 D_800BAB98;
extern Str12 D_800BAC6C;
extern Str28 D_800BAC7C;
extern Game *gGame[];
extern void *gUi[];
extern s32 D_800958AC;
extern s32 D_800958A8;
extern s32 D_800958B4;
extern u64 D_80097A00;
extern u8 D_80096EBC;
extern s8 D_800D31CC;
extern Player D_800958F0[];
extern s32 D_80095A70[];
extern s32 D_80095D04[];
extern s32 D_80095D34[];
extern s32 D_80095D64[];
extern u8 D_80095628;
extern u8 D_80097335;
extern Slot D_80130518[];
extern char gSaveSlotText[][0x32];
extern u64 D_80096EC8;
extern s32 D_80096EC0;

void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_AddSpriteElement(void *, UiDesc *);
void func_8005D8B8(void *, s32, s32);
void Msg_Queue(s32, s32, s32, s32);
void Pad_Rumble(s32, s32, s32);
s32 Race_FindCarSlot(s32);
u64 Time_Get(u64 *);
void Announcer_ClearQueue(void);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);

void Race_WaitForOthers(void) {
    s32 counts[4];
    UiDesc d1 = (PORT_CONVERT(&D_800BAC14, 18, 4), D_800BAC14); /* 18 s32, copied into locals */
    UiDesc d2 = (PORT_CONVERT(&D_800BAB50, 18, 4), D_800BAB50);
    Race *race;
    s32 i;
    s32 j;
    s32 idx;
    s32 total;

    Ui_ResetScreen(0x1135A, 0x1135B);
    D_80097A00 = 0;
    D_80096EBC = 0;
    race = gGame[0]->race;
    if (D_800958AC == 1) {
        S4 msgs = D_800BAC5C;
        for (j = 0; j < D_800D31CC; j++) {
            if (D_800958F0[j].x0->xD == 0) {
                Msg_Queue(msgs.v[j], 500, 1, 0);
                break;
            }
        }
    }
    for (i = 0; i < D_80095A70[0]; i++) {
        CarStat *st = gGame[0]->race->cars[i]->x38;
        if (gGame[0]->race->x10 == 0) {
            st->x10 = 12;
        } else {
            st->x10 = 6;
        }
    }
    for (i = 0; i < D_800D31CC; i++) {
        Pad_Rumble(i, 0, 0);
    }
    for (i = 0; i < D_800D31CC; i++) {
        idx = Race_FindCarSlot(D_800958F0[i].x0->x34);
        total = D_80095D04[idx] + D_80095D34[idx] + D_80095D64[idx];
        counts[i] = 0;
        for (j = 0; j < 12; j++) {
            if ((total < D_80095D04[j] + D_80095D34[j] + D_80095D64[j]) & (j != idx)) {
                counts[i]++;
            }
        }
        if (D_80095628 == 0) {
            gGame[0]->race->cars[i]->x3C->xC->xB4 = 7;
        }
    }
    if (D_800958A8 == 3) {
        d1.x14 = 0x11542;
        d1.x18 = 0x11543;
    } else if (D_800958A8 != 2 && counts[0] == 0 && race->x4 == 0 && D_800958A8 != 3) {
        d1.x14 = 0x11540;
        d1.x18 = 0x11541;
    } else if (D_800958A8 == 2 && D_80097335) {
        d1.x14 = 0x11540;
        d1.x18 = 0x11541;
        if (D_80130518[D_800958B4].x10 != 0) {
            Ui_AddElement(gUi[0], 2, 0x92);
        }
        if (D_80130518[D_800958B4].x8 != 0 && D_80130518[D_800958B4].x10 == 0) {
            Ui_AddElement(gUi[0], 2, 0x98);
        }
    } else {
        d1.x14 = 0x11542;
        d1.x18 = 0x11543;
    }
    Ui_AddSpriteElement(gUi[0], &d1);
    func_8005D8B8(gUi[0], 2, 0x20F);
    if (D_800958A8 == 2 || D_800958A8 == 3) {
        return;
    }
    if (race->x4 == 0) {
        S12 a = D_800BABA8;
        S12 b = D_800BABD8;
        S3 ann = D_800BAC08;
        *(Str13 *)gSaveSlotText[0] = D_800BAB98;
        Ui_AddElement(gUi[0], 0, 0x17A);
        d2.x14 = counts[0] * 2 + 0x11548;
        d2.x18 = counts[0] * 2 + 0x11549;
        Ui_AddSpriteElement(gUi[0], &d2);
        Ui_AddElement(gUi[0], 2, 0x210);
        D_80096EC8 = Time_Get(&gGame[0]->x1058);
        D_80096EC0 = -1;
        Announcer_ClearQueue();
        if (counts[0] == 0) {
            Announcer_QueueRandom(ann.v, 3, 500, 1, 0);
        } else if (race->x10 == 1) {
            Msg_Queue(a.v[counts[0]], 500, 1, 0);
        } else {
            Msg_Queue(b.v[counts[0]], 500, 1, 0);
        }
    } else {
        *(Str12 *)gSaveSlotText[0] = D_800BAC6C;
        *(Str28 *)gSaveSlotText[1] = D_800BAC7C;
        Ui_AddElement(gUi[0], 0, 0x17A);
        Ui_AddElement(gUi[0], 0, 0x17B);
        D_80096EC8 = (u64)((f64)Time_Get(&gGame[0]->x1058) + 3000000000.0);
        D_80096EC0 = counts[0];
    }
}
