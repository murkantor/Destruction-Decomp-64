#include "common.h"
#include "m2c.h"

M2C_UNK Race_Start(s32);                         /* extern */
M2C_UNK Race_Setup();                            /* extern */
M2C_UNK func_8005C780(s32);                         /* extern */
M2C_UNK func_8005D81C(s32);                         /* extern */
M2C_UNK Pad_LatchAll();                            /* extern */
M2C_UNK func_80063578(s16, u8);                     /* extern */
M2C_UNK func_80063890(M2C_UNK *);                   /* extern */
M2C_UNK Res_FreeAll();                            /* extern */
extern M2C_UNK gGameTime;
extern s32 gGame;
extern s16 D_8011EEEA;
extern u8 D_8011EEEC;
extern s32 gUi;
extern s32 D_8011EEF8;

void func_80063404(s32 arg0) {
    Res_FreeAll();
    func_80063578(D_8011EEEA, D_8011EEEC);
    Race_Start(arg0);
    Pad_LatchAll();
    func_80063890(&gGameTime);
    func_80063890(gGame + 0x1058);
    func_8005C780(D_8011EEF8);
    func_8005D81C(gUi);
    Race_Setup();
}
