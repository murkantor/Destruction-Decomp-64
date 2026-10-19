#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(s32);                         /* extern */
extern void *gGame;

void func_8002BED4(void) {
    void *temp_s0;

    temp_s0 = M2C_FIELD(gGame, void **, 0x1100);
    Mem_Free(M2C_FIELD(temp_s0, s32 *, 0x48));
    M2C_FIELD(temp_s0, s32 *, 0x48) = 0;
}
