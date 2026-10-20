#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x510]; s32 *x510; u8 pad2[0xC]; s32 x520; } Snd;
extern Snd *D_8011EEF8;
extern u8 D_80096EB4;
extern s32 D_800BC9E4;
extern s32 Sound_CountPlaying(s32);
extern void Res_LoadSoundBank(s32);
typedef struct { u8 pad[0x10]; s32 x10; } Obj;
s32 OptionsMenu_Update(Obj *o) {
    if (o->x10 == 0 || o->x10 == 3) {
        if (D_80096EB4 != 0 || D_800BC9E4 != D_8011EEF8->x520) {
            if (Sound_CountPlaying(2) == 0) {
                if (D_800BC9E4 == 12) {
                    Res_LoadSoundBank(0x759E);
                } else if (D_800BC9E4 == 13) {
                    Res_LoadSoundBank(0x75A0);
                } else {
                    Res_LoadSoundBank(D_8011EEF8->x510[D_800BC9E4]);
                }
                D_80096EB4 = 0;
                D_8011EEF8->x520 = D_800BC9E4;
            }
        }
    }
    return 1;
}
