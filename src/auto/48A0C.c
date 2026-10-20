#include "common.h"
#include "m2c.h"

typedef struct { s32 v[3]; } Ids3;
typedef struct { f32 v[3]; } Vol3;

typedef struct {
    u8 pad[0x514];
    Vol3 vol;
    s32 x520;
    u8 x524;
} Options;

typedef struct {
    u8 pad[0x10];
    s32 x10;
} Menu;

typedef struct {
    u8 pad[0x88];
    s32 x88;
} UiElem;

extern Options *D_8011EEF8;
extern Ids3 D_800BAD40;
extern Ids3 D_800BACF8;
extern Ids3 D_800BAD04;
extern void *gUi;
extern s32 D_8011EEE0;
extern f32 D_80096EB0;
extern char D_80123D82[];
extern char D_800BA87C[];
extern s32 D_800BC9E4;

void Timer_Start(Menu *, u64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
UiElem *Ui_FindElement(void *, s32, s32);
void Sound_SetVolume(s32, s32);
s32 Sound_CountPlaying(s32);
void Sound_FadeOutAll(s32, s32);
void Msg_Queue(s32, s32, s32, s32);
s32 sprintf(char *, const char *, ...);

static inline void SetSlider(s32 id, f32 v) {
    f32 k = 72.5f;
    UiElem *e = Ui_FindElement(gUi, 2, id);
    e->x88 = v * k + 2.0f;
    if (v == 1.0) {
        e->x88++;
    }
}

s32 OptionsMenu_Right(Menu *m) {
    Ids3 ids = D_800BAD40;
    Ids3 rem;
    Ids3 add;
    s32 *p0;
    s32 *p1;
    s32 *p2;
    Options *o;
    f32 f;
    f32 v;
    UiElem *e;
    s32 ret;

    ret = 1;
    Timer_Start(m, 200000000);
    switch (m->x10) {
        case 0: 
            o = D_8011EEF8;
            if (o->vol.v[0] == 2.0) {
                return 0;
            }
            rem = D_800BACF8;
            add = D_800BAD04;
            p0 = rem.v;
            if (o->vol.v[0] == 0.0) {
                Ui_RemoveElement(gUi, p0[m->x10]);
            }
            o->vol.v[0] += D_80096EB0;
            if (o->vol.v[0] >= 2.0) {
                o->vol.v[0] = 2.0f;
                Ui_AddElement(gUi, 2, (p0 + 4)[m->x10]);
            }
            f = D_8011EEF8->vol.v[0] * 128.0;
            if (f > 256.0) {
                f = 256.0f;
            }
            Sound_SetVolume(D_8011EEE0, f);
            SetSlider(ids.v[m->x10], D_8011EEF8->vol.v[0]);
            break;
        case 1: 
            o = D_8011EEF8;
            if (o->vol.v[1] == 2.0) {
                return 0;
            }
            rem = D_800BACF8;
            add = D_800BAD04;
            p1 = rem.v;
            if (o->vol.v[1] == 0.0) {
                Ui_RemoveElement(gUi, p1[m->x10]);
            }
            o->vol.v[1] += D_80096EB0;
            if (o->vol.v[1] >= 2.0) {
                o->vol.v[1] = 2.0f;
                Ui_AddElement(gUi, 2, (p1 + 4)[m->x10]);
            }
            SetSlider(ids.v[m->x10], D_8011EEF8->vol.v[1]);
            Msg_Queue(0xA9, 0, 0, 0);
            return ret;
        case 2: 
            o = D_8011EEF8;
            if (o->vol.v[2] == 2.0) {
                return 0;
            }
            rem = D_800BACF8;
            add = D_800BAD04;
            p2 = rem.v;
            if (o->vol.v[2] == 0.0) {
                Ui_RemoveElement(gUi, p2[m->x10]);
            }
            o->vol.v[2] += D_80096EB0;
            if (o->vol.v[2] >= 2.0) {
                o->vol.v[2] = 2.0f;
                Ui_AddElement(gUi, 2, (p2 + 4)[m->x10]);
            }
            SetSlider(ids.v[m->x10], D_8011EEF8->vol.v[2]);
            break;
        case 3:
            Timer_Start(m, 220000000);
            if (D_8011EEF8->x520 == 13) {
                D_800BC9E4 = 0;
            } else {
                D_800BC9E4 = D_8011EEF8->x520 + 1;
            }
            if (Sound_CountPlaying(2) != 0) {
                Sound_FadeOutAll(2, 0);
                while (Sound_CountPlaying(2) != 0) {
                }
            }
            sprintf(D_80123D82, D_800BA87C, D_800BC9E4 + 1);
            return ret;
        case 4:
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
    }
    return ret;
}
