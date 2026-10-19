#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[2];
    s16 x2;
    u8 pad4[0x38];
    s32 x3C;
} Snd;
typedef struct {
    u8 pad0[0x104];
    Snd *x104[4][60];
    u8 pad4C4[0x10];
    s32 x4D4[5];
} Bank;
extern Bank *D_8011EEF8;
void Sound_FadeOut(s32, s32);
void World_RemoveObject(Bank *, Snd *, s32, s32);

void func_80009A34(s32 idx, s32 id) {
    Snd *s = NULL;
    s32 i;

    for (i = 0; i < D_8011EEF8->x4D4[idx]; i++) {
        s = D_8011EEF8->x104[idx][i];
        if (s->x2 == id) {
            break;
        }
    }
    if (s != NULL) {
        if (s->x3C != 0) {
            Sound_FadeOut(s->x3C, 0);
        }
        World_RemoveObject(D_8011EEF8, s, idx, i);
    }
}
