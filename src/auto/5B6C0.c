#include "common.h"
#include "m2c.h"

M2C_UNK func_80057334(s32);                         /* extern */

void func_8005AAC0(void *arg0) {
    func_80057334(M2C_FIELD(arg0, s32 *, 4));
    M2C_FIELD(arg0, s32 *, 0) = 0;
}

void func_8005AAEC(void *arg0) {
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) | 2);
}

void func_8005AAFC(void *arg0) {
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFD);
}

void func_8005AB0C(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 9) = arg1;
}

void func_8005AB14(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0xA) = arg1;
}

void func_8005AB1C(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
    M2C_FIELD(arg0, s32 *, 0x14) = arg3;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

void func_8005AB34(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0x18) = arg1;
    M2C_FIELD(arg0, s32 *, 0x1C) = arg2;
    M2C_FIELD(arg0, s32 *, 0x20) = arg3;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

void func_8005AB4C(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x24) = arg1;
    M2C_FIELD(arg0, s8 *, 8) = 1;
}

s32 Pool_Alloc(M2C_UNK *);                       /* extern */
M2C_UNK func_80057334(s32);                         /* extern */
M2C_UNK guMtxF2L(M2C_UNK, s32);                /* extern */
M2C_UNK osWritebackDCache(s32, M2C_UNK);                /* extern */
extern M2C_UNK gParticlePool;

void func_8005AB5C(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;

    func_80057334(M2C_FIELD(arg0, s32 *, 4));
    temp_v0 = Pool_Alloc(&gParticlePool);
    M2C_FIELD(arg0, s32 *, 4) = temp_v0;
    guMtxF2L(arg1, temp_v0);
    osWritebackDCache(M2C_FIELD(arg0, s32 *, 4), 4);
    M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFE);
}
