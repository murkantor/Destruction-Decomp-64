#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x60];
    s32 x60;
    s32 x64;
    s32 x68;
    s32 x6C;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
extern GameState *gGame;
extern s32 D_801255E4;
extern s32 D_80125F50;
extern s32 D_80125BD0;
extern s32 D_80124358;
extern s32 D_80131380;
extern s32 D_80131F28;
extern RaceState *D_801238E8;
extern s32 D_8011F048[14];
extern s32 D_80125578[14];

void func_80013FBC(void) {
    s32 i;

    D_801255E4 = 0;
    D_80125F50 = 0;
    D_80125BD0 = 0;
    D_80124358 = 0;
    D_80131380 = 0;
    D_80131F28 = 0;
    D_801238E8 = gGame->race;
    D_801238E8->x64 = 0;
    D_801238E8->x60 = 0;
    D_801238E8->x6C = 0;
    D_801238E8->x68 = 0;
    for (i = 0; i < 14; i++) {
        D_80125578[i] = 0;
        D_8011F048[i] = 0;
    }
}
