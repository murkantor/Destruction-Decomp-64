typedef struct {
    u8 pad0[0x514];
    f32 x514;
} Settings;

extern void *gUi[];
extern Settings *D_8011EEF8;
extern s32 D_8011EEE0;
extern u8 D_80096EB4;
void Timer_Start(void *, s64);
void Ui_RemoveElement(void *, s32);
void Ui_AddElement(void *, s32, s32);
s32 Sound_SetVolume(s32, s32);
void Sound_FadeOutAll(s32, s32);
s32 Sound_CountPlaying(s32);

void func_8004736C(void *timer, s32 from, s32 to) {
    s32 ids[5] = { 0x59, 0x5C, 0x5F, 0x27, 0x62 };
    f32 vol;

    Timer_Start(timer, 200000000);
    Ui_RemoveElement(gUi[0], ids[from]);
    Ui_AddElement(gUi[0], 2, ids[to]);
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
