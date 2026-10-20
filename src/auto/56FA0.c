#include "common.h"
#include "m2c.h"

extern void *D_800B0B54;
extern s32 D_800B0B58;

void Fb_InitContext(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    M2C_FIELD(arg0, s32 *, 0x88) = arg1;
    M2C_FIELD(arg0, s32 *, 0x8C) = arg2;
    M2C_FIELD(arg0, s32 *, 0x90) = arg3;
    M2C_FIELD(arg0, s32 *, 0x98) = 0;
    M2C_FIELD(arg0, s32 *, 0x9C) = 0;
    M2C_FIELD(arg0, s32 *, 0xA0) = 0;
    M2C_FIELD(arg0, s32 *, 0xA4) = 0;
    M2C_FIELD(arg0, s32 *, 0xA8) = 0;
    M2C_FIELD(arg0, s32 *, 0xAC) = 0;
    M2C_FIELD(arg0, s32 *, 0xB4) = 0;
    M2C_FIELD(arg0, s32 *, 0xB8) = 0;
    D_800B0B54 = arg0;
    M2C_FIELD(arg0, s32 *, 0x94) = arg4;
    if (arg3 != 0) {
        D_800B0B58 = arg3;
    }
}
