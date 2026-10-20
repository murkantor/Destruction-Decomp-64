#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} Sub;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x28 - 0x14];
    Sub *x28;
} State;

typedef struct {
    u8 pad0[0x1058];
    u64 x1058;
} Game;

extern Game *gGame;
extern void *gUi;
extern u64 D_800964D8;
extern u8 D_800964E0;
extern s32 D_80124354;
extern u8 D_800972F0;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern char gSaveSlotText[][0x32];
u64 Time_Get(u64 *);
u8 Pak_Open(s32 *);
u8 Pak_HandleError(s32);
void Pak_LoadSave(s32 *);
void Pak_WriteSave(s32 *);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
void Game_SetNextMode(s32, s32);
void func_80063380(s32);

s32 PakMenu_AutoSaveTick(State *st) {
    if (st->x28->x10 != 0) {
        return 0;
    }
    if ((f64)(Time_Get(&gGame->x1058) - D_800964D8) > 1000000000.0 && !D_800964E0) {
        D_800964E0 = 1;
        if (Pak_Open(&D_80124354) && Pak_HandleError(D_80124354)) {
            if (D_8009734C) {
                Pak_LoadSave(&D_80124354);
            } else {
                Pak_WriteSave(&D_80124354);
            }
            Ui_AddElement(gUi, 0, 0x17D);
            if (Pak_HandleError(D_80124354)) {
                sprintf(gSaveSlotText[3], "     SUCCESSFUL");
                D_8009734C = 0;
                D_8009734D = 0;
            } else {
                sprintf(gSaveSlotText[3], "       ERROR");
            }
        }
    }
    if ((f64)(Time_Get(&gGame->x1058) - D_800964D8) > 3000000000.0) {
        if (D_80124354 == 0) {
            if (D_800972F0) {
                Game_SetNextMode(0x2E, 1);
            } else {
                func_80063380(2);
                Game_SetNextMode(0x19, 1);
                st->x10 = 0;
            }
        }
        D_800964E0 = 0;
    }
    return 0;
}
