#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
extern Vec3f D_800BA378;
extern Vec3f D_800BA384;
typedef struct { u8 pad[0xC]; s32 xC; } Pad;
extern u8 D_801253F8;
extern u8 D_80097A0C[];
extern u8 D_80096E6E;
extern struct { void *p; } D_8011EEF8;
extern void *gUi;
Pad *func_80062F14(void);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Sound_PlayAt(void *, Vec3f, Vec3f, s32);

s32 Menu_ToggleHelpOverlay(s32 arg) {
    s32 ret = 0;
    Vec3f a = D_800BA378;
    Vec3f b = D_800BA384;
    s32 i;

    if (arg != -1 && D_801253F8 == 0 && arg >= 12) {
        if (arg < 12 || D_80097A0C[arg] == 0) return 0;
    }
    if (func_80062F14()->xC & 0x2000) {
        if (D_80096E6E) return ret;
        Ui_AddElement(gUi, 2, 0x8F);
        for (i = 0; i < 6; i++) {
            Ui_AddElement(gUi, 0, 0x17A + i);
        }
        Ui_RemoveElement(gUi, 0x180);
        D_80096E6E = 1;
        ret = 1;
        Sound_PlayAt(D_8011EEF8.p, a, b, 7);
    } else {
        if (!D_80096E6E) return ret;
        Ui_RemoveElement(gUi, 0x8F);
        for (i = 0; i < 6; i++) {
            Ui_RemoveElement(gUi, 0x17A + i);
        }
        Ui_AddElement(gUi, 0, 0x180);
        ret = 1;
        D_80096E6E = 0;
        Sound_PlayAt(D_8011EEF8.p, a, b, 7);
    }
    return ret;
}
