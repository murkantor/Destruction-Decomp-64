#include "common.h"
#include "m2c.h"

struct huft {
    u8 e;
    u8 b;
    union {
        u16 n;
        struct huft *t;
    } v;
};

extern u32 D_800BCA10;
extern u32 D_800BCA14;
extern u32 D_800BCA18;
extern u8 *D_800BCA30;
extern const u16 gInflateMaskBits[];
s32 Inflate_GetByte(void);
void Inflate_FlushWindow(u8 *buf, u32 n);
void *memcpy(void *, const void *, u32);

#define NEEDBITS(n) { while (k < (n)) { s32 c = Inflate_GetByte(); if (c == 0x100) return 1; b |= ((u32)c) << k; k += 8; } }
#define DUMPBITS(n) { b >>= (n); k -= (n); }

s32 inflate_codes(struct huft *tl, struct huft *td, s32 bl, s32 bd) {
    register u32 e;
    u32 n, d;
    u32 w;
    struct huft *t;
    u32 ml, md;
    register u32 b;
    register u32 k;

    b = D_800BCA14;
    k = D_800BCA18;
    w = D_800BCA10;
    ml = gInflateMaskBits[bl];
    md = gInflateMaskBits[bd];
    for (;;) {
        NEEDBITS((u32)bl)
        if ((e = (t = tl + ((u32)b & ml))->e) > 16)
            do {
                if (e == 99)
                    return 1;
                DUMPBITS(t->b)
                e -= 16;
                NEEDBITS(e)
            } while ((e = (t = t->v.t + ((u32)b & gInflateMaskBits[e]))->e) > 16);
        DUMPBITS(t->b)
        if (e == 16) {
            D_800BCA30[w++] = (u8)t->v.n;
            if (w == 0x8000) {
                Inflate_FlushWindow(D_800BCA30, w);
                w = 0;
            }
        } else {
            if (e == 15)
                break;
            NEEDBITS(e)
            n = t->v.n + ((u32)b & gInflateMaskBits[e]);
            DUMPBITS(e);
            NEEDBITS((u32)bd)
            if ((e = (t = td + ((u32)b & md))->e) > 16)
                do {
                    if (e == 99)
                        return 1;
                    DUMPBITS(t->b)
                    e -= 16;
                    NEEDBITS(e)
                } while ((e = (t = t->v.t + ((u32)b & gInflateMaskBits[e]))->e) > 16);
            DUMPBITS(t->b)
            NEEDBITS(e)
            d = w - t->v.n - ((u32)b & gInflateMaskBits[e]);
            DUMPBITS(e)
            do {
                n -= (e = (e = 0x8000 - ((d &= 0x7FFF) > w ? d : w)) > n ? n : e);
                if (w - d >= e) {
                    memcpy(D_800BCA30 + w, D_800BCA30 + d, e);
                    w += e;
                    d += e;
                } else
                    do {
                        D_800BCA30[w++] = D_800BCA30[d++];
                    } while (--e);
                if (w == 0x8000) {
                    Inflate_FlushWindow(D_800BCA30, w);
                    w = 0;
                }
            } while (n);
        }
    }
    D_800BCA10 = w;
    D_800BCA14 = b;
    D_800BCA18 = k;
    return 0;
}
