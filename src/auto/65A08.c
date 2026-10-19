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

u8 *Seq_ReadEnvelope(Voice *v, u8 *p) {
    u8 n;

    n = *p++;
    if (n == 0) {
        n = 1;
    }
    v->xB7 = n;
    v->x60 = 1024 / n;
    v->xB8 = *p++;
    n = *p++;
    v->xBE = n;
    v->x54 = 1.0 / (f32)n * (f32)((v->xB9 = *p++) - v->xB8);
    n = *p++;
    v->xBF = n;
    v->x58 = 1.0 / (f32)n * (f32)((v->xBA = *p++) - v->xB9);
    n = *p++;
    v->xC0 = n;
    v->x5C = 1.0 / (f32)n;
    return p;
}
