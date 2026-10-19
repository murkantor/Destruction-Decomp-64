#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[8];
    u32 x8;
    u8 padC[8];
    u32 x14;
    u8 pad18[8];
    f32 x20;
    u8 pad24[0xC];
    u8 *x30;
    u8 pad34[0x34];
    f32 x68;
    f32 x6C;
    u8 pad70[0x2C];
    u16 x9C;
} Env670C0;

void Envelope_Step(Env670C0 *e) {
    u8 c;
    f32 v;
    s32 t;

    do {
        e->x14 += 0x100;
        if (--e->x9C == 0) {
            c = *e->x30++;
            if (c >= 0x80) {
                v = (f32)(c & 0x7F) - 64.0;
                e->x6C = v;
                e->x20 = v * e->x68;
                c = *e->x30++;
                if (c >= 0x80) {
                    t = (c & 0x7F) << 8;
                    e->x9C = t;
                    e->x9C = *e->x30++ + (t + 2);
                } else {
                    e->x9C = c + 2;
                }
            } else {
                v = (f32)c - 64.0;
                e->x9C = 1;
                e->x6C = v;
                e->x20 = v * e->x68;
            }
        }
    } while (e->x14 < e->x8);
}
