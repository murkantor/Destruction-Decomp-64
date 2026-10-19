#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(s32);                         /* extern */
extern void *D_8011EEE4;

void func_8002BED4(void) {
    void *temp_s0;

    temp_s0 = M2C_FIELD(D_8011EEE4, void **, 0x1100);
    Mem_Free(M2C_FIELD(temp_s0, s32 *, 0x48));
    M2C_FIELD(temp_s0, s32 *, 0x48) = 0;
}
