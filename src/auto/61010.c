#include "common.h"
#include "m2c.h"

void Sprite_DisableAlpha(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x10) = 0xFF;
    M2C_FIELD(arg0, s32 *, 4) = (s32) (M2C_FIELD(arg0, s32 *, 4) & 0xFFF7);
}
