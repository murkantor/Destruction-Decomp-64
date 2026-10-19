#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s16 xC; } Game;
typedef struct { u8 *x0; u8 pad4[0x38]; u8 x3C[0x2C]; } Car;
typedef struct { u8 *x0; u8 *x4; u8 *x8; u8 *xC; void *x10; } Unk123B58;
extern Game *gGame;
extern s8 D_800D31CC[];
extern Car D_800958F0[];
extern Unk123B58 D_80123B58[];

void func_80009638(void) {
    s32 i;
    char unused[1];

    if (gGame->xC == 2) {
        for (i = 0; i < D_800D31CC[0]; i++) {
            D_80123B58[i].x0 = D_800958F0[i].x0 + 0x218;
            D_80123B58[i].x4 = D_800958F0[i].x0 + 0x23C;
            D_80123B58[i].x8 = D_800958F0[i].x0 + 0x224;
            D_80123B58[i].xC = D_800958F0[i].x0;
            D_80123B58[i].x10 = D_800958F0[i].x3C;
        }
    }
}
