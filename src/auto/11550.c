#include "common.h"
#include "m2c.h"

void func_80010950(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x10) = arg1;
    M2C_FIELD(arg0, s32 *, 0x14) = arg2;
    M2C_FIELD(arg0, s32 *, 0x18) = arg3;
    M2C_FIELD(arg0, s32 *, 0x1C) = arg4;
    M2C_FIELD(arg0, s32 *, 0x20) = arg5;
    M2C_FIELD(arg0, s32 *, 0x24) = arg6;
    M2C_FIELD(arg0, s32 *, 0x28) = arg7;
}

void func_80010990(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x10) = 3;
    M2C_FIELD(arg0, s32 *, 0x14) = arg1;
}

void func_800109B0(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0x10) = 3;
    M2C_FIELD(arg0, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x14) = 0;
    M2C_FIELD(arg0, s32 *, 0x10) = 4;
    M2C_FIELD(arg0, s32 *, 0x14) = arg1;
    M2C_FIELD(arg0, s32 *, 0x18) = arg2;
}

M2C_UNK Pool_Free(M2C_UNK *, M2C_UNK *);        /* extern */
extern M2C_UNK gParticlePool;

void func_800109E0(void *arg0) {
    M2C_UNK *temp_a1;
    M2C_UNK *temp_a1_2;

    if (M2C_FIELD(arg0, M2C_UNK **, 8) != NULL) {
        do {
            temp_a1 = M2C_FIELD(arg0, M2C_UNK **, 8);
            M2C_FIELD(arg0, M2C_UNK **, 8) = (M2C_UNK *) *temp_a1;
            Pool_Free(&gParticlePool, temp_a1);
            M2C_FIELD(arg0, s32 *, 0xC) = (s32) (M2C_FIELD(arg0, s32 *, 0xC) - 1);
        } while (M2C_FIELD(arg0, M2C_UNK **, 8) != NULL);
    }
    if (M2C_FIELD(arg0, M2C_UNK **, 0) != NULL) {
        do {
            temp_a1_2 = M2C_FIELD(arg0, M2C_UNK **, 0);
            M2C_FIELD(arg0, M2C_UNK **, 0) = (M2C_UNK *) *temp_a1_2;
            Pool_Free(&gParticlePool, temp_a1_2);
            M2C_FIELD(arg0, s32 *, 4) = (s32) (M2C_FIELD(arg0, s32 *, 4) - 1);
        } while (M2C_FIELD(arg0, M2C_UNK **, 0) != NULL);
    }
}
