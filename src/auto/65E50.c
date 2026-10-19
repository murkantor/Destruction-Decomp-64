#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x30]; s32 x30; s32 x34; u8 pad38[0x6C - 0x38]; f32 x6C;
    u8 pad70[0x9A - 0x70]; u16 x9A; u16 x9C; u8 pad9E[0xB4 - 0x9E]; u8 xB4;
    u8 padB5[0xD3 - 0xB5]; u8 xD3; u8 *xD4[4]; s32 xE4[4]; s32 xF4[4];
    u16 x104[4]; u16 x10C[4]; u8 x114[4]; u8 x118[4]; u8 x11C[4];
} S;

u8 *Seq_PushCall(S *s, u8 *p) {
    s32 n = s->xD3;
    s->x114[n] = *p++;
    s->xD4[n] = p;
    s->xE4[n] = s->x34;
    s->xF4[n] = s->x30;
    s->x118[n] = s->xB4;
    s->x11C[n] = s->x6C;
    s->x104[n] = s->x9A;
    s->x10C[n] = s->x9C;
    s->xD3++;
    return p;
}
