#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x20];
    f32 x20;
    u8 pad24[0xC];
    u8 *x30;
    u8 *x34;
    u8 pad38[0x30];
    f32 x68;
    f32 x6C;
    u8 pad70[0x2A];
    u16 x9A;
    u16 x9C;
    u8 pad9E[0x16];
    u8 xB4;
    u8 padB5[0x1E];
    u8 xD3;
    u8 *xD4[4];
    u8 *xE4[4];
    u8 *xF4[4];
    u16 x104[4];
    u16 x10C[4];
    u8 x114[4];
    u8 x118[4];
    u8 x11C[4];
} Voice;

u8 *Voice_PopEnvelope(Voice *v, u8 *r) {
    s32 i = v->xD3 - 1;
    if (v->x114[i] != 0xFF) {
        if (--v->x114[i] == 0) {
            v->xD3 = i;
            i = -1;
        }
    }
    if (i >= 0) {
        r = v->xD4[i];
        v->x34 = v->xE4[i];
        v->x30 = v->xF4[i];
        v->xB4 = v->x118[i];
        v->x6C = v->x11C[i];
        v->x9A = v->x104[i];
        v->x9C = v->x10C[i];
        v->x20 = v->x6C * v->x68;
    }
    return r;
}
