#include "common.h"
#include "m2c.h"

M2C_UNK func_800084EC(s32);                         /* extern */
M2C_UNK func_80057308();                            /* extern */
M2C_UNK func_8005BDB8(void *);                      /* extern */
M2C_UNK func_8005FAE4(s32);                         /* extern */
M2C_UNK func_800633A0(s32);                         /* extern */
M2C_UNK func_80063404(s32);                         /* extern */
M2C_UNK func_80063488(s32, s16);                    /* extern */
M2C_UNK func_80063504(s32);                         /* extern */
M2C_UNK func_80065D60(M2C_UNK, M2C_UNK);            /* extern */
s32 func_80065DE0(M2C_UNK);                         /* extern */
M2C_UNK func_8006C0F0(M2C_UNK);                     /* extern */
extern u8 D_800D31F8;
extern void *D_8011EEE4;
extern s32 D_8011EEF0;
extern void *D_8011EEF8;

void func_80063070(void *arg0) {
    if (D_800D31F8 != 0) {
        func_8005FAE4(D_8011EEF0);
        func_80057308();
        func_80065D60(3, 0);
        do {

        } while (func_80065DE0(3) != 0);
        func_8006C0F0(0);
        return;
    }
    if ((M2C_FIELD(arg0, s16 *, 0x4BD28) != M2C_FIELD(D_8011EEE4, s16 *, 0xC)) || (M2C_FIELD(arg0, u8 *, 0x4BD3C) != 0)) {
        func_80057308();
        if (M2C_FIELD(D_8011EEE4, s16 *, 0xC) != -1) {
            func_80063504(M2C_FIELD(arg0, s32 *, 0x4BD24));
        }
        M2C_FIELD(arg0, u8 *, 0x4BD3C) = 0U;
        func_80063488(M2C_FIELD(arg0, s32 *, 0x4BD24), M2C_FIELD(arg0, s16 *, 0x4BD28));
        func_80063404(M2C_FIELD(arg0, s32 *, 0x4BD24));
    }
    if ((M2C_FIELD(D_8011EEF8, s32 *, 0x52C) != 0) && (func_80065DE0(2) == 0)) {
        func_800084EC(M2C_FIELD(D_8011EEF8, s32 *, 0x52C));
        M2C_FIELD(D_8011EEF8, s32 *, 0x52C) = 0;
    }
    func_800633A0(M2C_FIELD(arg0, s32 *, 0x4BD24));
    func_8005FAE4(D_8011EEF0);
    func_8005BDB8(D_8011EEF8);
}
