#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x334];
    u8 x334[0x10FE - 0x334];
    u8 x10FE;
} Game;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
} Entry4A574;

typedef struct {
    s32 x0;
    u8 pad4[0x64];
} Slot4A574;

typedef struct {
    u8 pad[0x10];
    s8 x10;
} Sub4A574;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x14];
    Sub4A574 *x28;
} Obj4A574;

extern Game *gGame;
extern s32 D_80097D48;
extern u8 D_80097D5C[];
extern char D_800BAF80[];
extern s32 D_800970F0;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern Entry4A574 D_80097D18[];
extern Slot4A574 D_80095908[];

s32 sprintf(char *, const char *, ...);
void HiScore_ApplyNewEntries(void);
void ObjList_Remove(void *, s32);
void Game_SetNextMode(s32, s32);
void Gfx_FlushFrames(void);

s32 func_8004A574(Obj4A574 *obj) {
    s32 i;

    if (obj->x28->x10 != D_80097D48) {
        return 0;
    }
    if (D_80097D5C[0] == 0) {
        sprintf((char *)D_80097D5C, D_800BAF80, obj->x28->x10 + 1);
    }
    HiScore_ApplyNewEntries();
    ObjList_Remove(gGame->x334, D_800970F0);
    if (D_800958A8 == 3 && D_800958AC == 2) {
        for (i = D_80097D48 + 1; i < 4; i++) {
            if (D_80097D18[i].x0 != -1 || D_80097D18[i].x4 != -1 || D_80097D18[i].x8 != -1) {
                D_80097D48 = i;
                gGame->x10FE = 1;
                D_80095908[i].x0 = 0;
                obj->x10 = 0;
                Game_SetNextMode(0x26, 1);
                Gfx_FlushFrames();
                return 1;
            }
        }
    }
    Game_SetNextMode(0x25, 1);
    return 1;
}
