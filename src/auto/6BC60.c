#include "common.h"
#include "m2c.h"

M2C_UNK func_8006B1A0();                            /* extern */
extern s32 D_800B56E0;

void func_8006B060(s32 arg0) {
    if (D_800B56E0 == 0) {
        D_800B56E0 = arg0;
        func_8006B1A0();
    }
}

M2C_UNK alSynDelete();                            /* extern */

void func_8006B08C(void) {
    if (D_800B56E0 != 0) {
        alSynDelete();
        D_800B56E0 = 0;
    }
}
