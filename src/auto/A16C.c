#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x40]; s32 x40; } Obj40;
typedef struct { u8 pad[0x4E4]; Obj40 *x4E4[9]; s32 x508; } A;
typedef struct { u8 pad[0x104]; Obj40 *x104[4][60]; u8 pad2[0x10]; s32 x4D4[4]; } B;
extern s8 D_800D31CC[];
extern B *D_8011EEF8[];

void World_FlagAllObjects(A *a) {
    char unused[1];
    s32 i, j;
    for (i = 0; i < a->x508; i++) {
        a->x4E4[i]->x40 = 1;
    }
    for (j = 0; j < D_800D31CC[0]; j++) {
        for (i = 0; i < D_8011EEF8[0]->x4D4[j]; i++) {
            D_8011EEF8[0]->x104[j][i]->x40 = 1;
        }
    }
}
