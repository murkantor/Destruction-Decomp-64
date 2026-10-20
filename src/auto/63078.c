#include "common.h"
#include "m2c.h"

void Sprite_SetMtxPosF(void *arg0, f32 arg1, f32 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x10) = (s16) (s32) (arg1 * 4.0f);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x12) = (s16) (s32) (arg2 * 4.0f);
}

void func_800624C4(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    void *temp_v0;

    temp_v0 = M2C_FIELD(arg0, void **, 0x18);
    M2C_FIELD(temp_v0, s32 *, 0) = (s32) (arg1 * 65536.0f);
    M2C_FIELD(temp_v0, s32 *, 4) = (s32) (arg2 * 65536.0f);
    M2C_FIELD(temp_v0, s32 *, 8) = (s32) (arg3 * 65536.0f);
    M2C_FIELD(temp_v0, s32 *, 0xC) = (s32) (arg4 * 65536.0f);
}

void func_80062520(void *arg0, f32 arg1) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x14) = (s16) (s32) ((1.0f / arg1) * 1024.0f);
}

void func_8006255C(void *arg0, f32 arg1) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x16) = (s16) (s32) ((1.0f / arg1) * 1024.0f);
}

void Sprite_SetBaseScaleF(void *arg0, f32 arg1, f32 arg2) {
    void *temp_v1;

    temp_v1 = M2C_FIELD(arg0, void **, 0x18);
    M2C_FIELD(temp_v1, s16 *, 0x14) = (s16) (s32) ((1.0f / arg1) * 1024.0f);
    M2C_FIELD(temp_v1, s16 *, 0x16) = (s16) (s32) ((1.0f / arg2) * 1024.0f);
}

void Sprite_SetPosF(void *arg0, f32 arg1, f32 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 0) = (s16) (s32) (arg1 * 4.0f);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 8) = (s16) (s32) (arg2 * 4.0f);
}

void Sprite_SetScaleF(void *arg0, f32 arg1, f32 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 2) = (s16) (s32) ((1.0f / arg1) * 1024.0f);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 0xA) = (s16) (s32) ((1.0f / arg2) * 1024.0f);
}

void func_800626A8(void *arg0, f32 arg1, f32 arg2) {
    void *temp_a1;

    temp_a1 = M2C_FIELD(arg0, void **, 0x14);
    M2C_FIELD(temp_a1, s16 *, 0) = (s16) ((s32) -((s32) ((s32) (arg1 * 65536.0f) * M2C_FIELD(temp_a1, u16 *, 4)) >> 9) / (s32) M2C_FIELD(temp_a1, u16 *, 2));
    M2C_FIELD(temp_a1, s16 *, 8) = (s16) ((s32) -((s32) ((s32) (arg2 * 65536.0f) * M2C_FIELD(temp_a1, u16 *, 0xC)) >> 9) / (s32) M2C_FIELD(temp_a1, u16 *, 0xA));
}
