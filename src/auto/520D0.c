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

extern u32 D_800BCA14;
extern u32 D_800BCA18;
extern u16 gInflateMaskBits[];
extern u32 D_800B0340[];
extern s32 D_800B0484;
extern s32 D_800B0488;
extern u16 D_800B038C[];
extern u16 D_800B03CC[];
extern u16 D_800B040C[];
extern u16 D_800B0448[];
s32 Inflate_GetByte(void);
s32 huft_build(u32 *b, u32 n, u32 s, u16 *d, u16 *e, struct huft **t, s32 *m);
s32 huft_free(struct huft *t);
s32 inflate_codes(struct huft *tl, struct huft *td, s32 bl, s32 bd);

#define NEEDBITS(n) { while (k < (n)) { s32 c = Inflate_GetByte(); if (c == 0x100) return 1; b |= ((u32)c) << k; k += 8; } }
#define DUMPBITS(n) { b >>= (n); k -= (n); }

s32 inflate_dynamic(void)
{
    s32 i;
    u32 j;
    u32 l;
    u32 m;
    u32 n;
    struct huft *tl;
    struct huft *td;
    s32 bl;
    s32 bd;
    u32 nb;
    u32 nl;
    u32 nd;
    u32 ll[288 + 32];
    register u32 b;
    register u32 k;

    b = D_800BCA14;
    k = D_800BCA18;

    NEEDBITS(5)
    nl = 257 + ((u32)b & 0x1f);
    DUMPBITS(5)
    NEEDBITS(5)
    nd = 1 + ((u32)b & 0x1f);
    DUMPBITS(5)
    NEEDBITS(4)
    nb = 4 + ((u32)b & 0xf);
    DUMPBITS(4)
    if (nl > 288 || nd > 32)
        return 1;

    for (j = 0; j < nb; j++) {
        NEEDBITS(3)
        ll[D_800B0340[j]] = (u32)b & 7;
        DUMPBITS(3)
    }
    for (; j < 19; j++)
        ll[D_800B0340[j]] = 0;

    bl = 7;
    i = huft_build(ll, 19, 19, NULL, NULL, &tl, &bl);
    if (bl == 0)
        i = 1;
    if (i != 0) {
        if (i == 1)
            huft_free(tl);
        return i;
    }

    n = nl + nd;
    m = gInflateMaskBits[bl];
    i = l = 0;
    while ((u32)i < n) {
        NEEDBITS((u32)bl)
        j = (td = tl + ((u32)b & m))->b;
        DUMPBITS(j)
        j = td->v.n;
        if (j < 16)
            ll[i++] = l = j;
        else if (j == 16) {
            NEEDBITS(2)
            j = 3 + ((u32)b & 3);
            DUMPBITS(2)
            if ((u32)i + j > n)
                return 1;
            for (j--; j != (u32)-1; j--)
                ll[i++] = l;
        } else if (j == 17) {
            NEEDBITS(3)
            j = 3 + ((u32)b & 7);
            DUMPBITS(3)
            if ((u32)i + j > n)
                return 1;
            for (j--; j != (u32)-1; j--)
                ll[i++] = 0;
            l = 0;
        } else {
            NEEDBITS(7)
            j = 11 + ((u32)b & 0x7f);
            DUMPBITS(7)
            if ((u32)i + j > n)
                return 1;
            for (j--; j != (u32)-1; j--)
                ll[i++] = 0;
            l = 0;
        }
    }

    huft_free(tl);

    D_800BCA14 = b;
    D_800BCA18 = k;

    bl = D_800B0484;
    i = huft_build(ll, nl, 257, D_800B038C, D_800B03CC, &tl, &bl);
    if (bl == 0)
        i = 1;
    if (i != 0) {
        if (i == 1)
            huft_free(tl);
        return i;
    }
    bd = D_800B0488;
    i = huft_build(ll + nl, nd, 0, D_800B040C, D_800B0448, &td, &bd);
    if (bd == 0 && nl > 257) {
        huft_free(tl);
        return 1;
    }
    if (i == 1)
        i = 0;
    if (i != 0) {
        huft_free(tl);
        return i;
    }

    if (inflate_codes(tl, td, bl, bd))
        return 1;

    huft_free(tl);
    huft_free(td);
    return 0;
}
