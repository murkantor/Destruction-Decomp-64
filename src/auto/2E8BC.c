#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x6C];
    s32 x6C;
    u8 pad70[8];
    u8 x78[0x28];
} Thing;
extern Thing *D_800960B0;
extern s32 D_800960B4;
void Mem_Free(void *);
void ModelInst_Free(void *);
void Res_ReleaseInstance(s32);

void Track_FreeProps(void) {
    s32 i;
    Thing *t;

    for (i = 0; i < D_800960B4; i++) {
        t = &D_800960B0[i];
        ModelInst_Free(t->x78);
        Res_ReleaseInstance(t->x6C);
    }
    if (D_800960B0 != NULL) {
        Mem_Free(D_800960B0);
        D_800960B0 = NULL;
    }
    D_800960B4 = 0;
}
