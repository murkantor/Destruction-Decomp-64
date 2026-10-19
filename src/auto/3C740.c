#include "common.h"
#include "m2c.h"

M2C_UNK func_8005D91C(s32, M2C_UNK, M2C_UNK);       /* extern */
extern s8 D_800964D0;
extern s32 D_8011EEF4;

void func_8003BB40(void) {
    func_8005D91C(D_8011EEF4, 2, 2);
    D_800964D0 = 0;
}

M2C_UNK func_80063380(M2C_UNK);                     /* extern */
M2C_UNK func_80063564(M2C_UNK, M2C_UNK);            /* extern */

s32 func_8003BB70(void) {
    func_80063380(0);
    func_80063564(2, 1);
    return 1;
}
