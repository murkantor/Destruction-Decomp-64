#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x40];
    s32 id;
    u8 pad44[0x7F];
    u8 xC3;
    u8 xC4;
    u8 padC5[0x5B];
} Voice;
extern s32 gNumVoices;
extern Voice *gVoices;

s32 Sound_SetPanById(s32 id, s32 pan) {
    char unused[1];
    s32 i;
    s32 n;
    Voice *v;

    if (id == 0) {
        return 0;
    }
    if (pan < 0) {
        pan = 0;
    } else if (pan > 127) {
        pan = 127;
    }
    n = 0;
    v = gVoices;
    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->id == id) {
            n++;
            v->xC3 = pan;
            v->xC4 = 0xFF;
        }
    }
    return n;
}
