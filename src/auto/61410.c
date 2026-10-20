#include "common.h"
#include "m2c.h"

void func_80060810(void) {

}

void Sprite_SetPos(void *arg0, s16 arg1, s16 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 0) = arg1;
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 8) = arg2;
}

void Sprite_SetScale(void *arg0, s16 arg1, s16 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 2) = arg1;
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s16 *, 0xA) = arg2;
}

void Sprite_SetMtxPos(void *arg0, s16 arg1, s16 arg2) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x10) = arg1;
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x18), s16 *, 0x12) = arg2;
}

void Sprite_SetMtx(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    void *temp_v0;

    temp_v0 = M2C_FIELD(arg0, void **, 0x18);
    M2C_FIELD(temp_v0, s32 *, 0) = arg1;
    M2C_FIELD(temp_v0, s32 *, 4) = arg2;
    M2C_FIELD(temp_v0, s32 *, 8) = arg3;
    M2C_FIELD(temp_v0, s32 *, 0xC) = arg4;
}
