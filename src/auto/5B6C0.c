#include "common.h"
#include "m2c.h"

M2C_UNK Gfx_FreeAfterFrame(s32);                         /* extern */

void ModelInst_Free(void *arg0) {
    Gfx_FreeAfterFrame(M2C_FIELD(arg0, s32 *, 4));
    M2C_FIELD(arg0, s32 *, 0) = 0;
}

void ModelInst_SetXlu(void *arg0) {
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) | 2);
}

void ModelInst_ClearXlu(void *arg0) {
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFD);
}

void ModelInst_SetAlpha(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 9) = arg1;
}

void ModelInst_SetShade(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0xA) = arg1;
}

void ModelInst_SetPos(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
    M2C_FIELD(arg0, s32 *, 0x14) = arg3;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

void ModelInst_SetRot(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0x18) = arg1;
    M2C_FIELD(arg0, s32 *, 0x1C) = arg2;
    M2C_FIELD(arg0, s32 *, 0x20) = arg3;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

void ModelInst_SetScale(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x24) = arg1;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

s32 Pool_Alloc(M2C_UNK *);                       /* extern */
M2C_UNK Gfx_FreeAfterFrame(s32);                         /* extern */
M2C_UNK guMtxF2L(M2C_UNK, s32);                /* extern */
M2C_UNK osWritebackDCache(s32, M2C_UNK);                /* extern */
extern M2C_UNK gParticlePool;

void ModelInst_SetMatrix(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;

    Gfx_FreeAfterFrame(M2C_FIELD(arg0, s32 *, 4));
    temp_v0 = Pool_Alloc(&gParticlePool);
    M2C_FIELD(arg0, s32 *, 4) = temp_v0;
    guMtxF2L(arg1, temp_v0);
    osWritebackDCache(M2C_FIELD(arg0, s32 *, 4), 4);
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFE);
}
