#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */
f32 Time_ToSeconds(s32);                             /* extern */
extern s32 gGame;
extern void *gUi;
extern f32 D_80125BD4;

void func_8005F884(void *arg0) {
    s32 temp_v0;
    s32 temp_v0_2;
    void *temp_s2;
    void *var_a0;
    void *var_s0;

    if (M2C_FIELD(arg0, u8 *, 0x8B68) != 0) {
        temp_s2 = gUi;
        var_s0 = M2C_FIELD(temp_s2, void **, 0x8B54);
        var_a0 = var_s0;
        if (var_s0 != NULL) {
            do {
                var_s0 = M2C_FIELD(var_s0, void **, 4);
                Mem_Free(var_a0);
                var_a0 = var_s0;
            } while (var_s0 != NULL);
        }
        M2C_FIELD(temp_s2, s32 *, 0x8B54) = 0;
        temp_v0 = M2C_FIELD(arg0, s32 *, 0x8B6C);
        M2C_FIELD(arg0, s32 *, 0x8B6C) = 0;
        M2C_FIELD(arg0, u8 *, 0x8B68) = 0U;
        M2C_FIELD(arg0, s32 *, 0x8B54) = temp_v0;
        return;
    }
    temp_v0_2 = M2C_FIELD(arg0, s32 *, 0x8B54);
    M2C_FIELD(arg0, s32 *, 0x8B54) = 0;
    M2C_FIELD(arg0, u8 *, 0x8B68) = 1U;
    M2C_FIELD(arg0, s32 *, 0x8B6C) = temp_v0_2;
    D_80125BD4 = Time_ToSeconds(gGame + 0x1058);
}

void Font_EnableAlpha(void *arg0, s16 arg1) {
    M2C_FIELD(arg0, s16 *, 0x1C) = arg1;
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) | 1);
}

void Font_DisableAlpha(void *arg0) {
    M2C_FIELD(arg0, s16 *, 0x1C) = 0xFF;
    M2C_FIELD(arg0, s32 *, 0) = (s32) (M2C_FIELD(arg0, s32 *, 0) & 0xFFFE);
}

void Font_SetAlphaValue(void *arg0, s16 arg1) {
    M2C_FIELD(arg0, s16 *, 0x1C) = arg1;
}
