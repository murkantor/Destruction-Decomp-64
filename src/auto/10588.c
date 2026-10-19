#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s16 xC; u8 padE[0x10FC - 0xE]; s16 x10FC; } Game;
typedef struct { u8 x0; u8 pad[0x27]; } A28;
typedef struct { u8 x0; u8 pad[0x4F]; } A50;
typedef struct { s64 x0; u8 pad8[0xC]; s8 x14; u8 pad15[0xB]; } A20;
extern Game *gGame;
extern u8 D_80125F94;
extern s8 D_800D31CC[];
extern A28 D_80125FE8[];
extern A50 D_8011EF11[];
extern A20 D_8007EF70[2][10];
void MapUi_Init(s32);
void Hud_ResetTimeStrings(void);
void Race_GetLapTimeLimits(s32);

void func_8000F988(s32 a0, s32 a1) {
    s32 i;
    s32 j;

    D_80125F94 = 0;
    if (gGame->xC == 0) {
        goto end;
    }
    if (gGame->xC <= 0) {
        return;
    }
    if (gGame->xC == 1) {
        goto end;
    }
    if (gGame->xC != 2) {
        return;
    }
    if (gGame->x10FC == 0x19) {
        goto end;
    }
    MapUi_Init(a0);
    Hud_ResetTimeStrings();
    for (i = 0; i < D_800D31CC[0]; i++) {
        D_80125FE8[i].x0 = 0;
    }
    for (i = 0; i < D_800D31CC[0]; i++) {
        D_8011EF11[i].x0 = 0;
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 10; j++) {
            D_8007EF70[i][j].x14 = -1;
            D_8007EF70[i][j].x0 = 0;
        }
    }
end:
    Race_GetLapTimeLimits(a1);
}
