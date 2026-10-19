#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x54];
    f32 x54;
    f32 x58;
    f32 x5C;
    s32 x60;
    u8 pad64[0xC];
    s32 x70;
    u8 pad74[0x1C];
    s16 x90;
    u8 pad92[2];
    s16 x94;
    u8 pad96[8];
    u16 x9E;
    s16 xA0;
    u8 padA2[0x15];
    u8 xB7;
    u8 xB8;
    u8 xB9;
    u8 xBA;
    u8 padBB[3];
    u8 xBE;
    u8 xBF;
    u8 xC0;
    u8 padC1[0x5E];
} Voice;
extern s32 gNumVoices;
extern Voice *gVoices;
extern s32 D_800CCAAC;

u8 *Seq_SetTempo(Voice *v, u8 *p) {
    char unused[1];
    s32 tempo;
    s32 vol;
    s32 i;
    Voice *o;

    tempo = (*p++ * 3 * 8192) / 120 / D_800CCAAC;
    vol = (tempo * v->x90) >> 7;
    if (v->x9E) {
        v->x94 = tempo;
    } else {
        o = gVoices;
        for (i = 0; i < gNumVoices; i++, o++) {
            if (o->x70 == v->x70) {
                o->xA0 = tempo;
                o->x94 = vol;
            }
        }
    }
    return p;
}
