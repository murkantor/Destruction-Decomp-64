#include "common.h"
#include "m2c.h"

void *Res_Find();                              /* extern */
M2C_UNK List_Remove(void *, s32);                 /* extern */

void func_80063AAC(void) {
    s32 *temp_a0;
    void *temp_v0;

    temp_v0 = Res_Find();
    if (temp_v0 != NULL) {
        if (M2C_FIELD(temp_v0, s32 *, 0x1C) >= 2) {
            temp_a0 = M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4);
            if (temp_a0 != NULL) {
                M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(*temp_a0);
                List_Remove(temp_v0 + 0x14, *M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4));
            }
            M2C_FIELD(temp_v0, s32 *, 0x1C) = (s32) (M2C_FIELD(temp_v0, s32 *, 0x1C) - 1);
            return;
        }
        M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        List_Remove(temp_v0 + 0x14, M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        M2C_FIELD(temp_v0, s32 *, 0x1C) = 0;
    }
}

void *Res_FindType();                              /* extern */
M2C_UNK List_Remove(void *, s32);                 /* extern */

void func_80063B54(s32 arg0) {
    s32 *temp_a0;
    void *temp_v0;

    temp_v0 = Res_FindType();
    if (temp_v0 != NULL) {
        if (M2C_FIELD(temp_v0, s32 *, 0x1C) >= 2) {
            temp_a0 = M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4);
            if (temp_a0 != NULL) {
                M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(*temp_a0);
                List_Remove(temp_v0 + 0x14, arg0);
            }
            M2C_FIELD(temp_v0, s32 *, 0x1C) = (s32) (M2C_FIELD(temp_v0, s32 *, 0x1C) - 1);
            return;
        }
        M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        List_Remove(temp_v0 + 0x14, arg0);
        M2C_FIELD(temp_v0, s32 *, 0x1C) = 0;
    }
}
