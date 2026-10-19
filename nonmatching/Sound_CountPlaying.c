typedef struct {
    s32 active;
    u8 pad4[8];
    s32 fadeTime;
    u8 pad10[8];
    s32 fadeSteps;
    u8 pad1C[0x10];
    f32 pitch;
    u8 pad30[0x10];
    s32 id;
    u8 pad44[0x44];
    f32 basePitch;
    u8 pad8C[0xA];
    s16 volume;
    u8 pad98[6];
    u16 isMusic;
    u8 padA0[8];
    s16 xA8;
    u8 padAA[0xC];
    u8 xB6;
    u8 padB7[0x69];
} Voice;
extern s32 gNumVoices;
extern Voice *gVoices;

s32 Sound_CountPlaying(s32 mask) {
    s32 i;
    s32 count = 0;
    s32 n = gNumVoices;
    Voice *v = gVoices;

    for (i = 0; i < n; i++, v++) {
        if (v->active) {
            if (v->isMusic) {
                if (mask & 1) {
                    count++;
                }
            } else if (mask & 2) {
                count++;
            }
        }
    }
    return count;
}
