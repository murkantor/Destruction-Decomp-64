#include "common.h"
#include "m2c.h"

M2C_UNK Race_Start(s32);                         /* extern */
M2C_UNK Race_Setup();                            /* extern */
M2C_UNK World_ResetObjects(s32);                         /* extern */
M2C_UNK func_8005D81C(s32);                         /* extern */
M2C_UNK Pad_LatchAll();                            /* extern */
M2C_UNK Game_SetMode(s16, u8);                     /* extern */
M2C_UNK Clock_Reset(M2C_UNK *);                   /* extern */
M2C_UNK Res_FreeAll();                            /* extern */
extern M2C_UNK gGameTime;
extern s32 gGame;
extern s16 D_8011EEEA;
extern u8 D_8011EEEC;
extern s32 gUi;
extern s32 D_8011EEF8;

void Game_StartMode(s32 arg0) {
    Res_FreeAll();
    Game_SetMode(D_8011EEEA, D_8011EEEC);
    Race_Start(arg0);
    Pad_LatchAll();
    Clock_Reset(&gGameTime);
    Clock_Reset(gGame + 0x1058);
    World_ResetObjects(D_8011EEF8);
    func_8005D81C(gUi);
    Race_Setup();
}
