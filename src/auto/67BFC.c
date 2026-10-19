#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[8]; u32 x8; u8 padC[4]; u32 x10; u8 pad14[0x20]; u8 *x34; u8 pad38[0x62]; u16 x9A; u8 pad9C[0x18]; u8 xB4; } Seq66FFC;

void Seq_AdvanceDelta(Seq66FFC *s) {
    u8 b;

    do {
        s->x10 += 0x100;
        if (--s->x9A == 0) {
            b = *s->x34++;
            if (b >= 0x80) {
                s->xB4 = b & 0x7F;
                b = *s->x34++;
                if (b >= 0x80) {
                    s->x9A = (b & 0x7F) << 8;
                    s->x9A += *s->x34++ + 2;
                } else {
                    s->x9A = b + 2;
                }
            } else {
                s->xB4 = b;
                s->x9A = 1;
            }
        }
    } while (s->x10 < s->x8);
}
