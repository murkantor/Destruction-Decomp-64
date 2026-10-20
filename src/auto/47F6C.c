#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { u8 pad[0x514]; f32 x514; } Settings;
typedef struct { s32 v[5]; } Arr5;

extern s32 D_80097350;
extern void *gUi;
/* func_8004736C reads gUi as an array, which keeps its address in a register */
extern void *gUiArr[] ASM_ALIAS("gUi");
extern Settings *D_8011EEF8;
extern s32 D_8011EEE0;
extern u8 D_80096EB4;

void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
s32 Sound_SetVolume(s32, s32);
void Sound_FadeOutAll(s32, s32);
s32 Sound_CountPlaying(s32);

const Arr5 D_800BAD10 = { { 0x59, 0x5C, 0x5F, 0x27, 0x62 } };

void func_8004736C(void *timer, s32 from, s32 to) {
    Arr5 ids;
    f32 vol;

    ids = D_800BAD10;
    Timer_Start(timer, 200000000);
    Ui_RemoveElement(gUiArr[0], ids.v[from]);
    Ui_AddElement(gUiArr[0], 2, ids.v[to]);
    if (to == 0 || to == 3) {
        vol = D_8011EEF8->x514 * 128.0;
        D_80096EB4 = 1;
        if (vol < 0.0) {
            vol = 0.0f;
        }
        Sound_SetVolume(D_8011EEE0, (s32)vol);
    }
    if (from == 0 || from == 3) {
        Sound_FadeOutAll(2, 0);
        while (Sound_CountPlaying(2) != 0) {
        }
    }
}


s32 OptionsMenu_Prev(Menu *m) {
    s32 next;
    s32 cur;
    Arr5 ids;

    if (m->x10 != 0) {
        next = m->x10 - 1;
    } else {
        next = D_80097350 - 1;
    }
    cur = m->x10;
    ids = D_800BAD10;
    Timer_Start(m, 200000000);
    Ui_RemoveElement(gUi, ids.v[cur]);
    Ui_AddElement(gUi, 2, ids.v[next]);
    if (next == 0 || next == 3) {
        f32 vol = D_8011EEF8->x514 * 128.0;
        D_80096EB4 = 1;
        if (vol < 0.0) {
            vol = 0.0f;
        }
        Sound_SetVolume(D_8011EEE0, (s32)vol);
    }
    if (cur == 0 || cur == 3) {
        Sound_FadeOutAll(2, 0);
        while (Sound_CountPlaying(2) != 0) {
        }
    }
    m->x10 = next;
    return 1;
}

typedef struct { u8 pad[0x10]; s32 x10; } Obj;

s32 OptionsMenu_Next(Obj *arg0) {
    u32 next;
    u32 cur;
    Arr5 t;
    f32 vol;

    if (arg0->x10 == D_80097350 - 1) {
        next = 0;
    } else {
        next = arg0->x10 + 1;
    }
    cur = arg0->x10;
    t = D_800BAD10;
    Timer_Start(arg0, 200000000);
    Ui_RemoveElement(gUi, t.v[cur]);
    Ui_AddElement(gUi, 2, t.v[next]);
    if (next == 0 || next == 3) {
        vol = D_8011EEF8->x514 * 128.0;
        D_80096EB4 = 1;
        if (vol < 0.0) {
            vol = 0.0f;
        }
        Sound_SetVolume(D_8011EEE0, (s32)vol);
    }
    if (cur == 0 || cur == 3) {
        Sound_FadeOutAll(2, 0);
        while (Sound_CountPlaying(2) != 0) {
        }
    }
    arg0->x10 = next;
    return 1;
}
