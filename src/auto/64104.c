#include "common.h"
#include "m2c.h"

M2C_UNK func_8002C134(s32);                         /* extern */
M2C_UNK func_8005C6A0(s32);                         /* extern */
M2C_UNK func_8005D640(s32);                         /* extern */
M2C_UNK func_8005FAC8(s32);                         /* extern */
M2C_UNK func_80063CD8();                            /* extern */
M2C_UNK func_80064364(s32);                         /* extern */
extern s32 D_8011EEF0;
extern s32 D_8011EEF4;
extern s32 D_8011EEF8;

void func_80063504(s32 arg0) {
    func_8005C6A0(D_8011EEF8);
    func_80064364(arg0 + 0x334);
    func_8002C134(arg0);
    func_8005D640(D_8011EEF4);
    func_8005FAC8(D_8011EEF0);
    func_80063CD8();
}
