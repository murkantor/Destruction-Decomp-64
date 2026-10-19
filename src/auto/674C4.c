#include "common.h"
#include "m2c.h"

M2C_UNK alSynStopVoice(M2C_UNK *, s32);              /* extern */
M2C_UNK alSynStartVoice(M2C_UNK *, s32, s32);         /* extern */
extern s32 D_800CCA9C;
extern M2C_UNK D_8011F2B8;

void func_800668C4(void *arg0, s32 arg1) {
    if (M2C_FIELD(arg0, u8 *, 0xC1) != 0) {
        alSynStopVoice(&D_8011F2B8, D_800CCA9C + (arg1 * 0x1C));
    }
    M2C_FIELD(arg0, u8 *, 0xC1) = 1U;
    alSynStartVoice(&D_8011F2B8, D_800CCA9C + (arg1 * 0x1C), M2C_FIELD(arg0, s32 *, 4));
    M2C_FIELD(arg0, s32 *, 4) = 0;
}
