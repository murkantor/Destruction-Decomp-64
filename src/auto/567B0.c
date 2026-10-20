#include "common.h"
#include "m2c.h"

void PtrList_Init(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
}

M2C_UNK *Pool_Alloc(M2C_UNK *);                  /* extern */
extern M2C_UNK gParticlePool;

void PtrList_Add(M2C_UNK *arg0, s32 arg1) {
    M2C_UNK *temp_v0;
    M2C_UNK *var_s0;
    s32 temp_v0_2;

    var_s0 = arg0;
    if (*var_s0 != NULL) {
        do {
            var_s0 = *var_s0;
        } while (*var_s0 != NULL);
    }
    if (M2C_FIELD(var_s0, s32 *, 4) == 0xE) {
        temp_v0 = Pool_Alloc(&gParticlePool);
        M2C_FIELD(temp_v0, M2C_UNK **, 0) = NULL;
        M2C_FIELD(temp_v0, s32 *, 4) = 0;
        M2C_FIELD(var_s0, M2C_UNK **, 0) = temp_v0;
        var_s0 = temp_v0;
    }
    temp_v0_2 = M2C_FIELD(var_s0, s32 *, 4);
    M2C_FIELD(var_s0, s32 *, 4) = (s32) (temp_v0_2 + 1);
    M2C_FIELD(&var_s0[temp_v0_2], s32 *, 8) = arg1;
}
