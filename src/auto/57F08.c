#include "common.h"
#include "m2c.h"

M2C_UNK Gfx_SubmitTask(M2C_UNK, M2C_UNK, M2C_UNK);   /* extern */
extern s32 D_800B0B58;

void Gfx_FlushFrames(void) {
    D_800B0B58 = 0;
    Gfx_SubmitTask(0, 2, 0);
}

s32 Pool_Alloc(M2C_UNK *);                       /* extern */
M2C_UNK PtrList_Init(s32);                         /* extern */
M2C_UNK PtrList_Add(s32, s32);                    /* extern */
extern s32 D_800B0B68;
extern M2C_UNK gParticlePool;

void Gfx_FreeAfterFrame(s32 arg0) {
    s32 var_s0;

    if (D_800B0B68 != 0) {
        var_s0 = D_800B0B68;
    } else {
        var_s0 = Pool_Alloc(&gParticlePool);
        PtrList_Init(var_s0);
        D_800B0B68 = var_s0;
    }
    PtrList_Add(var_s0, arg0);
}
