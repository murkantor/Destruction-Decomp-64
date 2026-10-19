#include "common.h"
#include "m2c.h"

M2C_UNK func_8002C318(s32);                         /* extern */
M2C_UNK func_8002C6F0();                            /* extern */
M2C_UNK func_8005C780(s32);                         /* extern */
M2C_UNK func_8005D81C(s32);                         /* extern */
M2C_UNK func_80062E90();                            /* extern */
M2C_UNK func_80063578(s16, u8);                     /* extern */
M2C_UNK func_80063890(M2C_UNK *);                   /* extern */
M2C_UNK func_80063CD8();                            /* extern */
extern M2C_UNK D_800D31D0;
extern s32 D_8011EEE4;
extern s16 D_8011EEEA;
extern u8 D_8011EEEC;
extern s32 D_8011EEF4;
extern s32 D_8011EEF8;

void func_80063404(s32 arg0) {
    func_80063CD8();
    func_80063578(D_8011EEEA, D_8011EEEC);
    func_8002C318(arg0);
    func_80062E90();
    func_80063890(&D_800D31D0);
    func_80063890(D_8011EEE4 + 0x1058);
    func_8005C780(D_8011EEF8);
    func_8005D81C(D_8011EEF4);
    func_8002C6F0();
}
