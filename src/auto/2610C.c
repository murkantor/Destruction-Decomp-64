#include "common.h"
#include "m2c.h"

M2C_UNK LensFlare_Free(s32);                         /* extern */
M2C_UNK List_FreeAll(void *);                      /* extern */
M2C_UNK Mem_Free(void *);                      /* extern */

void Race_Free(void *arg0) {
    s32 temp_a0;

    if (M2C_FIELD(arg0, s32 *, 0x20) != 0) {
        List_FreeAll(arg0 + 0x20);
    }
    if (M2C_FIELD(arg0, s32 *, 0x14) != 0) {
        List_FreeAll(arg0 + 0x14);
    }
    temp_a0 = M2C_FIELD(arg0, s32 *, 0xA8);
    if (temp_a0 != 0) {
        LensFlare_Free(temp_a0);
    }
    Mem_Free(arg0);
}
