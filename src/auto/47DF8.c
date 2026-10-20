#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} State;

typedef struct {
    u8 pad0[0x514];
    f32 x514;
} Settings;

typedef struct {
    s32 x0;
    u8 pad4[0x68 - 0x4];
} Slot68;

extern void *gUi[];
extern Settings *D_8011EEF8;
extern s32 D_8011EEE0;
extern u8 D_80097335[];
extern u8 D_80097336;
extern Slot68 D_80095908[];
void Ui_RemoveElement(void *, s32);
s32 Sound_SetVolume(s32, s32);
void Sound_FadeOutAll(s32, s32);
s32 Sound_CountPlaying(s32);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 func_800471F8(State *st) {
    f32 vol;
    s8 i;

    if (st->x10 == 1) {
        if (Sound_CountPlaying(2) != 0) {
            Sound_FadeOutAll(2, 0);
            while (Sound_CountPlaying(2) != 0) {
            }
        }
        vol = D_8011EEF8->x514 * 128.0;
        if (vol < 0.0) {
            vol = 0.0f;
        }
        Sound_SetVolume(D_8011EEE0, (s32)vol);
        if (D_80097335[0]) {
            D_80097335[0] = 0;
        }
        if (D_80097336) {
            D_80097336 = 0;
        }
        func_80063380(1);
        Game_SetNextMode(0x13, 1);
    } else {
        Ui_RemoveElement(gUi[0], 0x17A);
        Ui_RemoveElement(gUi[0], 0x17C);
        Ui_RemoveElement(gUi[0], 0x17D);
        for (i = 0; i < 4; i++) {
            D_80095908[i].x0 = 1;
        }
        Game_SetNextMode(0x1F, 1);
    }
    return 1;
}
