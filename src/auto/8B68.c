#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 pad8[8]; } SfxPair;
extern SfxPair D_801311D0[];
void func_8005CACC(void *, s16);

void Sfx_StopPair(void *ctx, s32 i) {
    SfxPair *s = &D_801311D0[i];

    if (s->x0 != 0) {
        func_8005CACC(ctx, s->x0);
        s->x0 = 0;
    }
    if (s->x4 != 0) {
        func_8005CACC(ctx, s->x4);
        s->x4 = 0;
    }
}
