#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 *x4;
} SeqHdr;

typedef struct {
    s32 x0;
    u8 pad4[0x44 - 0x4];
    s32 x44;
    u8 pad48[0x70 - 0x48];
    SeqHdr *x70;
    u8 pad74[0x78 - 0x74];
    s32 x78;
    u8 pad7C[0x9E - 0x7C];
    u16 x9E;
    u8 padA0[0x120 - 0xA0];
} Voice;

extern Voice *gVoices;
extern s32 gNumVoices;

s32 Voice_FindFree(SeqHdr *h, s32 ch) {
    s32 i;
    s32 bi;
    s32 best;
    Voice *v;

    for (i = 0, v = gVoices; i < gNumVoices; i++, v++) {
        if (v->x0 == 0) {
            return i;
        }
    }
    v = gVoices;
    best = 0x7FFFFFFF;
    bi = -1;
    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->x9E != 0 && v->x44 <= best) {
            best = v->x44;
            bi = i;
        }
    }
    if (bi != -1) {
        return bi;
    }
    v = gVoices;
    for (i = 0; i < gNumVoices; i++) {
        if (v->x9E == 0 && v->x70 != h) {
            return i;
        }
    }
    for (i = 0, v = gVoices; i < gNumVoices; i++, v++) {
        if (v->x70 == h && h->x4[ch] == v->x78) {
            return i;
        }
    }
    return ch % gNumVoices;
}
