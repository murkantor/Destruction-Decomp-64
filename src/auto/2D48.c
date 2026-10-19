#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(s32);                         /* extern */
extern void *D_8011EEE4;

void func_80002148(void) {
    void *temp_s0;

    temp_s0 = M2C_FIELD(D_8011EEE4, void **, 0x1100);
    func_80055AB4(M2C_FIELD(temp_s0, s32 *, 0x3C));
    M2C_FIELD(temp_s0, s32 *, 0x3C) = 0;
}
