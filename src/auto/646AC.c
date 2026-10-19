#include "common.h"
#include "m2c.h"

void *func_80063E78();                              /* extern */
M2C_UNK func_80063FEC(void *, s32);                 /* extern */

void func_80063AAC(void) {
    s32 *temp_a0;
    void *temp_v0;

    temp_v0 = func_80063E78();
    if (temp_v0 != NULL) {
        if (M2C_FIELD(temp_v0, s32 *, 0x1C) >= 2) {
            temp_a0 = M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4);
            if (temp_a0 != NULL) {
                M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(*temp_a0);
                func_80063FEC(temp_v0 + 0x14, *M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4));
            }
            M2C_FIELD(temp_v0, s32 *, 0x1C) = (s32) (M2C_FIELD(temp_v0, s32 *, 0x1C) - 1);
            return;
        }
        M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        func_80063FEC(temp_v0 + 0x14, M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        M2C_FIELD(temp_v0, s32 *, 0x1C) = 0;
    }
}

void *func_80063ED0();                              /* extern */
M2C_UNK func_80063FEC(void *, s32);                 /* extern */

void func_80063B54(s32 arg0) {
    s32 *temp_a0;
    void *temp_v0;

    temp_v0 = func_80063ED0();
    if (temp_v0 != NULL) {
        if (M2C_FIELD(temp_v0, s32 *, 0x1C) >= 2) {
            temp_a0 = M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 **, 4);
            if (temp_a0 != NULL) {
                M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(*temp_a0);
                func_80063FEC(temp_v0 + 0x14, arg0);
            }
            M2C_FIELD(temp_v0, s32 *, 0x1C) = (s32) (M2C_FIELD(temp_v0, s32 *, 0x1C) - 1);
            return;
        }
        M2C_FIELD(temp_v0, M2C_UNK (**)(s32), 0x10)(M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x14), s32 *, 0));
        func_80063FEC(temp_v0 + 0x14, arg0);
        M2C_FIELD(temp_v0, s32 *, 0x1C) = 0;
    }
}
