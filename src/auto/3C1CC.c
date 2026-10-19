#include "common.h"
#include "m2c.h"

M2C_UNK func_8000E864();                            /* extern */
M2C_UNK func_8005D91C(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_80063890(s32);                         /* extern */
M2C_UNK func_800638C0(s32);                         /* extern */
M2C_UNK func_800638C8(s32);                         /* extern */
extern s32 gGame;
extern s32 D_8011EEF4;

void func_8003B5CC(void) {
    func_8005D91C(D_8011EEF4, 2, 0x1F);
    func_8000E864();
    func_800638C8(gGame + 0x10A8);
    func_80063890(gGame + 0x10A8);
    func_800638C0(gGame + 0x10A8);
}
