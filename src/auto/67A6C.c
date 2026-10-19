#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x8]; u32 x8; u8 pad1[0x80]; u32 x8C; u8 pad2[0x25]; u8 xB5; u8 pad3[0x16]; u8 xCC; u8 pad4[4]; u8 xD1; u8 xD2; } Snd;
void Snd_UpdateVolumeRamp(Snd *s) {
    u8 step = s->xCC;
    u32 v;
    do {
        v = s->xD1 + step;
        s->x8C += 0x100;
        if (v < 0x40) {
            s->xD1 = v;
        } else {
            s->xD1 = v & 0x3F;
            v >>= 6;
            if (s->xD2 == 0) {
                s->xB5 += v;
                if (s->xB5 >= 0x80) {
                    s->xB5 = 0x7F;
                    s->xD2 = 1;
                }
            } else {
                s->xB5 -= v;
                if (s->xB5 >= 0x80 || s->xB5 == 0) {
                    s->xB5 = 0;
                    s->xD2 = 0;
                }
            }
        }
    } while (s->x8C < s->x8);
}
