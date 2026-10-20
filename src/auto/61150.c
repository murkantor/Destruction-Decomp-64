#include "common.h"
#include "m2c.h"

s32 Mem_AllocAligned(M2C_UNK, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK Sprite_InitMtx(void *);                      /* extern */

void func_80060550(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x18) = Mem_AllocAligned(0x18, 0x3F, 0);
    Sprite_InitMtx(arg0);
}
