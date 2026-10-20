#include "common.h"
#include "m2c.h"

void Sprite_SetAlphaValue(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x10) = arg1;
}
