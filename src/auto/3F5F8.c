#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                           /* extern */
M2C_UNK ModelInst_Free(s32);                         /* extern */
M2C_UNK Res_Release(s32);                         /* extern */
extern M2C_UNK D_800919F4;

s32 CarObj_Free(void *arg0) {
    void *temp_s0;

    if (arg0 != NULL) {
        temp_s0 = M2C_FIELD(arg0, void **, 0xC);
        ModelInst_Free(temp_s0 + 0x98);
        ModelInst_Free(temp_s0 + 0xC0);
        ModelInst_Free(temp_s0 + 0xE8);
        ModelInst_Free(temp_s0 + 0x110);
        ModelInst_Free(temp_s0 + 0x138);
        Res_Release(*((__typeof__(D_800919F4) *)((s8 *)&D_800919F4 + (M2C_FIELD(temp_s0, s32 *, 0x34) * 4))));
        Mem_Free(temp_s0);
        Mem_Free(arg0);
    }
    return 0;
}
