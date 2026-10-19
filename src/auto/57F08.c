#include "common.h"
#include "m2c.h"

M2C_UNK Debug_DrawPerfStats(M2C_UNK, M2C_UNK, M2C_UNK);   /* extern */
extern s32 D_800B0B58;

void func_80057308(void) {
    D_800B0B58 = 0;
    Debug_DrawPerfStats(0, 2, 0);
}

s32 Pool_Alloc(M2C_UNK *);                       /* extern */
M2C_UNK func_80055BB0(s32);                         /* extern */
M2C_UNK func_80055BBC(s32, s32);                    /* extern */
extern s32 D_800B0B68;
extern M2C_UNK gParticlePool;

void func_80057334(s32 arg0) {
    s32 var_s0;

    if (D_800B0B68 != 0) {
        var_s0 = D_800B0B68;
    } else {
        var_s0 = Pool_Alloc(&gParticlePool);
        func_80055BB0(var_s0);
        D_800B0B68 = var_s0;
    }
    func_80055BBC(var_s0, arg0);
}
