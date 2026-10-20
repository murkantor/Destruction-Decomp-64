#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x1058]; u8 x1058[8]; } Game;
typedef struct { Game *game; u8 pad4[0xC]; void *ui; } Globals;
extern Globals gGame;
extern void *gUi;
typedef struct { char c[2]; } Str2;
extern Str2 D_800BAF30;
extern s32 D_80124560[];
extern u64 D_80124568;
extern s32 D_80124574;
extern s32 D_80124578;
extern char gSaveSlotText[];
extern u8 D_800972F0;
u64 Time_Get(void *);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
char *strcpy(char *, const char *);
void Ui_EnableElementAlpha(void *, s32, s32, s32);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);
void Res_Release(s32);

s32 func_8004F4E4(void) {
    Str2 str;
    u64 t;
    s32 i;
    char *p;
    Globals *g = &gGame;
    void **pu;

    str = D_800BAF30;
    t = Time_Get(g->game->x1058);
    switch ((u32)D_80124560[0]) {
    case 0:
        Ui_RemoveElement(gUi, 0xA3);
        Ui_RemoveElement(gUi, 0xA5);
        Ui_AddElement(gUi, 2, 0xA4);
        D_80124560[0] = 1;
        D_80124568 = t;
        D_80124578 = 0;
        D_80124574 = 0;
        i = 0;
        pu = &g->ui;
        p = gSaveSlotText;
        for (; i < 10; i++) {
            Ui_AddElement(*pu, 0, i + 0x17A);
            strcpy(p, str.c);
            Ui_EnableElementAlpha(*pu, 0, i + 0x17A, 0);
            p += 0x32;
        }
        return 1;
    case 1:
        D_80124568 = t;
        return 1;
    case 2:
        func_80063380(2);
        Game_SetNextMode(0x19, 1);
        Res_Release(0x11426);
        D_800972F0 = 0;
        return 1;
    }
    return 1;
}
