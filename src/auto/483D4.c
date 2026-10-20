#include "common.h"
#include "m2c.h"

typedef struct { s32 v[3]; } Ids;
typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { u8 pad[0x88]; s32 x88; } UiElem;
typedef struct {
    u8 pad[0x514];
    f32 x514;
    f32 x518;
    f32 x51C;
    s32 x520;
    u8 x524;
} Options;

extern Ids D_800BAD40;
extern Ids D_800BACF8;
extern Ids D_800BAD04;
extern Options *D_8011EEF8;
extern void *D_8011EEE0;
extern f32 D_80096EB0;
extern void *gUi;
extern s32 D_800BC9E4;
extern char D_80123D82[];
extern char D_800BA87C[];
extern void Timer_Start(void *, s64);
extern void Ui_RemoveElement(void *, s32);
extern void Ui_AddElement(void *, s32, s32);
extern UiElem *Ui_FindElement(void *, s32, s32);
extern void Sound_SetVolume(void *, s32);
extern void Msg_Queue(s32, s32, s32, s32);
extern s32 Sound_CountPlaying(s32);
extern void Sound_FadeOutAll(s32, s32);
extern s32 sprintf(char *, const char *, ...);

static inline void Slider_Dec(Menu *m, f32 *val) {
    Ids on;
    Ids off;

    on = D_800BACF8;
    off = D_800BAD04;
    if (*val == 2.0) {
        Ui_RemoveElement(gUi, off.v[m->x10]);
    }
    *val -= D_80096EB0;
    if (*val <= 0.0) {
        *val = 0;
        Ui_AddElement(gUi, 2, on.v[m->x10]);
    }
}

static inline void Slider_Set(s32 id, f32 v, f32 w) {
    UiElem *el = Ui_FindElement(gUi, 2, id);
    el->x88 = v * w + 2.0f;
    if (v == 1.0) {
        el->x88++;
    }
}

s32 OptionsMenu_Left(Menu *m) {
    Ids ids = D_800BAD40;
    s32 ret = 1;
    Options *o;
    f32 v;
    f32 w;
    UiElem *el;

    Timer_Start(m, 200000000);
    switch (m->x10) {
        case 0:
            o = D_8011EEF8;
            if (o->x514 == 0.0) {
                return 0;
            }
            Slider_Dec(m, &o->x514);
            {
                f32 vol = D_8011EEF8->x514 * 128.0;
                if (vol < 0.0) {
                    vol = 0;
                }
                Sound_SetVolume(D_8011EEE0, vol);
            }
            Slider_Set(ids.v[m->x10], D_8011EEF8->x514, 72.5f);
            break;
        case 1:
            o = D_8011EEF8;
            if (o->x518 == 0.0) {
                return 0;
            }
            Slider_Dec(m, &o->x518);
            Slider_Set(ids.v[m->x10], D_8011EEF8->x518, 72.5f);
            Msg_Queue(0xA9, 0, 0, 0);
            break;
        case 2:
            o = D_8011EEF8;
            if (o->x51C == 0.0) {
                return 0;
            }
            Slider_Dec(m, &o->x51C);
            Slider_Set(ids.v[m->x10], D_8011EEF8->x51C, 72.5f);
            break;
        case 3:
            Timer_Start(m, 220000000);
            if (D_8011EEF8->x520 == 0) {
                D_800BC9E4 = 13;
            } else {
                D_800BC9E4 = D_8011EEF8->x520 - 1;
            }
            if (Sound_CountPlaying(2)) {
                Sound_FadeOutAll(2, 0);
                while (Sound_CountPlaying(2)) {
                }
            }
            sprintf(D_80123D82, D_800BA87C, D_800BC9E4 + 1);
            break;
        case 4:
            Timer_Start(m, 220000000);
            if (D_8011EEF8->x524) {
                D_8011EEF8->x524 = 0;
                Ui_RemoveElement(gUi, 0x64);
                Ui_AddElement(gUi, 2, 0x63);
            } else {
                D_8011EEF8->x524 = 1;
                Ui_RemoveElement(gUi, 0x63);
                Ui_AddElement(gUi, 2, 0x64);
            }
            break;
        default:
            ret = 0;
            break;
    }
    return ret;
}
