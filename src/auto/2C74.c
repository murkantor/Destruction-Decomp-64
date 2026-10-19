#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(s32);                         /* extern */
extern void *gGame;

void func_80002074(void) {
    void *temp_s0;

    temp_s0 = M2C_FIELD(gGame, void **, 0x1100);
    Mem_Free(M2C_FIELD(temp_s0, s32 *, 0x30));
    M2C_FIELD(temp_s0, s32 *, 0x30) = 0;
    Mem_Free(M2C_FIELD(temp_s0, s32 *, 0x34));
    M2C_FIELD(temp_s0, s32 *, 0x34) = 0;
}
