#include "common.h"
#include "m2c.h"

void func_80061524(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0x44) = arg1;
    M2C_FIELD(arg0, s32 *, 0x48) = arg2;
    M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) | 4);
}

void func_8006153C(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x44) = 0;
    M2C_FIELD(arg0, s32 *, 0x48) = 0;
    M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) & 0xFFFB);
}

void Bg_EnableAlpha(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x4C) = arg1;
    M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) | 8);
}

void Bg_DisableAlpha(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x4C) = 0xFF;
    M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) & 0xFFF7);
}

void Bg_SetAlphaValue(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x4C) = arg1;
}

void func_80061588(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x50) = arg1;
}

void func_80061590(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x54) = arg1;
}

void func_80061598(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s16 *, 0) = (s16) (arg1 << 5);
    M2C_FIELD(arg0, s16 *, 8) = (s16) (arg2 << 5);
}
