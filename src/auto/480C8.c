#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { u8 pad[0x514]; f32 x514; } Settings;
typedef struct { s32 v[5]; } Arr5;

extern Arr5 D_800BAD10;
extern f64 D_800BAD30;
extern s32 D_80097350;
extern void *gUi;
extern Settings *D_8011EEF8;
extern s32 D_8011EEE0;
extern u8 D_80096EB4;

void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
s32 Sound_SetVolume(s32, s32);
void Sound_FadeOutAll(s32, s32);
s32 Sound_CountPlaying(s32);

s32 Menu_PrevPage(Menu *m) {
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
        f32 vol = D_8011EEF8->x514 * D_800BAD30;
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
extern const f64 D_800BAD38;

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
        vol = D_8011EEF8->x514 * D_800BAD38;
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
