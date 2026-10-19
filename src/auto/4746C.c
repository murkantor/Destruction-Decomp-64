#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { u8 pad[0x510]; s32 *x510; f32 x514; u8 pad518[8]; s32 x520; } Settings;
typedef struct { u8 pad[0x1058]; u8 x1058[0x28]; u8 x1080[4]; } Game;

extern Game *gGame[];
extern void *gUi;
extern Settings *D_8011EEF8;
extern s32 D_8011EEE0;
extern s32 D_80125C38;
extern f32 D_80125BD4;
extern f64 D_800BACC0;

void Rumble_DisallowRetry(void);
void func_80062E4C(void);
void func_80063564(s32, s32);
void func_800638C0(void *);
void func_800638C8(void *);
void func_8005CC98(void *);
void Ui_RemoveElement(void *, s32);
s32 Sound_CountPlaying(s32);
void func_800084EC(s32);
s32 Sound_SetVolume(s32, s32);
f32 Time_ToSeconds(void *);

s32 func_8004686C(Menu *m) {
    switch (m->x10) {
        case 0: {
            f32 vol;
            Rumble_DisallowRetry();
            func_80062E4C();
            func_80063564(D_80125C38, 0);
            func_800638C0(gGame[0]->x1058);
            func_800638C8(gGame[0]->x1080);
            func_8005CC98(D_8011EEF8);
            Ui_RemoveElement(gUi, 0x38);
            Ui_RemoveElement(gUi, 0x39);
            Ui_RemoveElement(gUi, 0x1A7);
            Ui_RemoveElement(gUi, 0x1A6);
            Ui_RemoveElement(gUi, 0x1B0);
            Ui_RemoveElement(gUi, 0x1B1);
            if (Sound_CountPlaying(2) == 0) {
                if (D_8011EEF8->x520 == 12) {
                    func_800084EC(0x759E);
                } else if (D_8011EEF8->x520 == 13) {
                    func_800084EC(0x75A0);
                } else {
                    func_800084EC(D_8011EEF8->x510[D_8011EEF8->x520]);
                }
            }
            vol = D_8011EEF8->x514 * D_800BACC0;
            if (vol < 0.0) {
                vol = 0.0f;
            }
            Sound_SetVolume(D_8011EEE0, (s32)vol);
            D_80125BD4 = Time_ToSeconds(gGame[0]->x1058);
            break;
        }
        case 1:
            func_80063564(0x21, 1);
            break;
        case 2:
            if (Sound_CountPlaying(2) == 0) {
                if (D_8011EEF8->x520 == 12) {
                    func_800084EC(0x759E);
                } else if (D_8011EEF8->x520 == 13) {
                    func_800084EC(0x75A0);
                } else {
                    func_800084EC(D_8011EEF8->x510[D_8011EEF8->x520]);
                }
            }
            func_80063564(0x20, 1);
            break;
        case 3:
            func_80063564(0x27, 1);
            break;
    }
    m->x10 = 0;
    return 1;
}
