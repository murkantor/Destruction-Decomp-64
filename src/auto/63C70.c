#include "common.h"
#include "m2c.h"

M2C_UNK func_800084EC(s32);                         /* extern */
M2C_UNK func_80057308();                            /* extern */
M2C_UNK World_UpdateSounds(void *);                      /* extern */
M2C_UNK func_8005FAE4(s32);                         /* extern */
M2C_UNK func_800633A0(s32);                         /* extern */
M2C_UNK func_80063404(s32);                         /* extern */
M2C_UNK func_80063488(s32, s16);                    /* extern */
M2C_UNK func_80063504(s32);                         /* extern */
M2C_UNK Sound_FadeOutAll(M2C_UNK, M2C_UNK);            /* extern */
s32 Sound_CountPlaying(M2C_UNK);                         /* extern */
M2C_UNK __osSpSetPc(M2C_UNK);                     /* extern */
extern u8 D_800D31F8;
extern void *gGame;
extern s32 D_8011EEF0;
extern void *D_8011EEF8;

void func_80063070(void *arg0) {
    if (D_800D31F8 != 0) {
        func_8005FAE4(D_8011EEF0);
        func_80057308();
        Sound_FadeOutAll(3, 0);
        do {

        } while (Sound_CountPlaying(3) != 0);
        __osSpSetPc(0);
        return;
    }
    if ((M2C_FIELD(arg0, s16 *, 0x4BD28) != M2C_FIELD(gGame, s16 *, 0xC)) || (M2C_FIELD(arg0, u8 *, 0x4BD3C) != 0)) {
        func_80057308();
        if (M2C_FIELD(gGame, s16 *, 0xC) != -1) {
            func_80063504(M2C_FIELD(arg0, s32 *, 0x4BD24));
        }
        M2C_FIELD(arg0, u8 *, 0x4BD3C) = 0U;
        func_80063488(M2C_FIELD(arg0, s32 *, 0x4BD24), M2C_FIELD(arg0, s16 *, 0x4BD28));
        func_80063404(M2C_FIELD(arg0, s32 *, 0x4BD24));
    }
    if ((M2C_FIELD(D_8011EEF8, s32 *, 0x52C) != 0) && (Sound_CountPlaying(2) == 0)) {
        func_800084EC(M2C_FIELD(D_8011EEF8, s32 *, 0x52C));
        M2C_FIELD(D_8011EEF8, s32 *, 0x52C) = 0;
    }
    func_800633A0(M2C_FIELD(arg0, s32 *, 0x4BD24));
    func_8005FAE4(D_8011EEF0);
    World_UpdateSounds(D_8011EEF8);
}
