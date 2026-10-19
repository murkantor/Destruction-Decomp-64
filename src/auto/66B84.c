#include "common.h"
#include "m2c.h"

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

s32 Sound_SetFieldA8(s32 id, s16 x) {
    s32 i;
    s32 count;
    Voice *v;

    if (id == 0) {
        return 0;
    }
    count = 0;
    v = gVoices;
    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->id == id) {
            count++;
            v->xA8 = x;
            v->xB6 = 0xFF;
        }
    }
    return count;
}
