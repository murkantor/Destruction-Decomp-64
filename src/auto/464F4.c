#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1058]; u64 x1058; u8 pad1060[0x1100 - 0x1060]; struct Info *x1100; } Game;
typedef struct Info { u8 pad[0x10]; s32 x10; } Info;
typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
    u8 pad1C[0x48 - 0x1C];
} UiDesc;
typedef struct { char s[13]; } Str13;
typedef struct { s32 v[12]; } List12;
typedef struct { s32 v[3]; } List3;

extern Game *gGame[];
extern void *gUi[];
extern Str13 gSaveSlotText;
extern s32 D_80096EC0;
extern u64 D_80096EC8;
extern u8 D_80096EBC;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern u32 D_800958B4;
extern s32 D_800958BC;
extern UiDesc D_800BAB50;
extern Str13 D_800BAB98;
extern List12 D_800BABA8;
extern List12 D_800BABD8;
extern List3 D_800BAC08;
u64 Time_Get(u64 *);
void Ui_RemoveElement(void *, s32);
void Ui_AddSpriteElement(void *, UiDesc *);
void Ui_AddElement(void *, s32, s32);
void Announcer_ClearQueue(void);
u8 Game_TimeBefore(void);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);
void Msg_Queue(s32 id, s32 arg1, u8 force, u8 once);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 Race_UpdateFinish(void) {
    Info *info = gGame[0]->x1100;
    s32 id;

    if (D_80096EC0 != -1) {
        if (D_80096EC8 < Time_Get(&gGame[0]->x1058)) {
            UiDesc d = D_800BAB50;
            gSaveSlotText = D_800BAB98;
            Ui_RemoveElement(gUi[0], 0x17B);
            d.x14 = D_80096EC0 * 2 + 0x11548;
            d.x18 = D_80096EC0 * 2 + 0x11549;
            Ui_AddSpriteElement(gUi[0], &d);
            Ui_AddElement(gUi[0], 2, 0x210);
            if (D_800958A8 != 3) {
                List12 a = D_800BABA8;
                List12 b = D_800BABD8;
                List3 c = D_800BAC08;
                Announcer_ClearQueue();
                if (D_80096EC0 == 0) {
                    Announcer_QueueRandom(c.v, 3, 500, 1, 0);
                } else if (info->x10 == 1) {
                    Msg_Queue(a.v[D_80096EC0], 500, 1, 0);
                } else {
                    Msg_Queue(b.v[D_80096EC0], 500, 1, 0);
                }
            }
            D_80096EC0 = -1;
        }
        return 1;
    }
    if (D_80096EBC == 0) {
        return 1;
    }
    if (Game_TimeBefore() != 0) {
        return 1;
    }
    if (D_800958A8 == 2) {
        func_80063380(1);
        Game_SetNextMode(0x2A, 1);
    } else if (D_800958A8 == 3) {
        switch ((enum E { E0, E1, E2, E3 })D_800958AC) {
            case 0:
                func_80063380(1);
                Game_SetNextMode(0x22, 1);
                break;
            case 1:
                D_800958BC++;
                func_80063380(1);
                Game_SetNextMode(0x2B, 1);
                break;
            case 2:
                func_80063380(1);
                Game_SetNextMode(0x2C, 1);
                break;
            case 3:
                func_80063380(1);
                Game_SetNextMode(0x23, 1);
                break;
        }
    } else if (D_800958B4 < 4) {
        func_80063380(1);
        Game_SetNextMode(0x22, 1);
    } else {
        func_80063380(1);
        Game_SetNextMode(0x23, 1);
    }
    return 1;
}
