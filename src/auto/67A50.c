#include "common.h"
#include "m2c.h"

void Snd_StartAutoPan(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0xD1) = 0;
    M2C_FIELD(arg0, s32 *, 0x8C) = (s32) M2C_FIELD(arg0, s32 *, 0x3C);
    M2C_FIELD(arg0, s8 *, 0xD2) = (s8) (M2C_FIELD(arg0, u8 *, 0xB5) & 0x40);
}
