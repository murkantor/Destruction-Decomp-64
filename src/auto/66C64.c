#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x40];
    s32 id;
    u8 pad44[0x4C];
    s16 x90;
    u8 pad92[2];
    s16 x94;
    u8 pad96[0xA];
    u16 xA0;
    u8 padA2[0x7E];
} Voice;
extern s32 gNumVoices;
extern Voice *gVoices;

s32 Sound_SetTempoById(s32 id, s32 vol) {
    char unused[1];
    s32 i;
    s32 n;
    Voice *v;

    if (id == 0) {
        return 0;
    }
    if (vol <= 0) {
        vol = 1;
    } else if (vol > 256) {
        vol = 256;
    }
    n = 0;
    v = gVoices;
    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->id == id) {
            n++;
            v->x90 = vol;
            v->x94 = (v->xA0 * vol) >> 7;
        }
    }
    return n;
}
