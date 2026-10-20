#include "common.h"
#include "m2c.h"

M2C_UNK guS2DInitBg();                            /* extern */
extern f64 D_800BBF00;

void Bg_SetScale(void *arg0, f32 arg1, f32 arg2, s32 arg3) {
    if (((f64) arg1 == D_800BBF00) && ((f64) arg2 == D_800BBF00)) {
        M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) & 0xFFFE);
        guS2DInitBg(arg0);
        return;
    }
    M2C_FIELD(arg0, s32 *, 0x20) = arg3;
    M2C_FIELD(arg0, s32 *, 0x40) = (s32) (M2C_FIELD(arg0, s32 *, 0x40) | 1);
    M2C_FIELD(arg0, s16 *, 0x1C) = (s16) (s32) (arg1 * 1024.0f);
    M2C_FIELD(arg0, s16 *, 0x1E) = (s16) (s32) (arg2 * 1024.0f);
}
