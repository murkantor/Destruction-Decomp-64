#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(s32);                         /* extern */
void *func_8005E620(s32, M2C_UNK, M2C_UNK);         /* extern */
extern s32 D_800D1AEC;
extern s32 D_8011EEF4;

void func_8000E960(void) {
    void *temp_v0;

    temp_v0 = func_8005E620(D_8011EEF4, 2, 0x1F);
    func_80055AB4(*M2C_FIELD(temp_v0, s32 **, 0x10));
    *M2C_FIELD(temp_v0, s32 **, 0x10) = 0;
    D_800D1AEC = 0;
}
