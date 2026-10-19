#include "common.h"
#include "m2c.h"

M2C_UNK func_8002BF30(s32);                         /* extern */
M2C_UNK func_80057308();                            /* extern */
M2C_UNK func_8005BB60(s32);                         /* extern */
M2C_UNK func_80063578(s16, u8);                     /* extern */
extern void *gGame;
extern s16 D_8011EEEA;
extern u8 D_8011EEEC;
extern s32 D_8011EEF8;

void func_800633A0(s32 arg0) {
    if (D_8011EEEA != M2C_FIELD(gGame, s16 *, 0x10FC)) {
        func_80063578(D_8011EEEA, D_8011EEEC);
        func_80057308();
    }
    func_8002BF30(arg0);
    func_8005BB60(D_8011EEF8);
}
