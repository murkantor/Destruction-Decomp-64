#include "common.h"
#include "m2c.h"

extern u32 D_800BCA10;
extern u32 D_800BCA14;
extern u32 D_800BCA18;
extern u8 *D_800BCA30;
s32 Inflate_GetByte(void);
void func_80052370(u8 *buf, u32 n);

#define NEEDBITS(n) { while (k < (n)) { s32 c = Inflate_GetByte(); if (c == 0x100) return 1; b |= ((u32)c) << k; k += 8; } }
#define DUMPBITS(n) { b >>= (n); k -= (n); }

s32 Inflate_Stored(void) {
    s32 n;
    u32 w;
    u32 b;
    u32 k;

    b = D_800BCA14;
    k = D_800BCA18;
    w = D_800BCA10;

    n = k & 7;
    DUMPBITS(n);

    NEEDBITS(16)
    n = (b & 0xffff);
    DUMPBITS(16)
    NEEDBITS(16)
    if (n != ((~b) & 0xffff))
        return 1;
    DUMPBITS(16)

    for (n--; n != -1; n--) {
        NEEDBITS(8)
        D_800BCA30[w++] = (u8)b;
        if (w == 0x8000) {
            func_80052370(D_800BCA30, w);
            w = 0;
        }
        DUMPBITS(8)
    }

    D_800BCA10 = w;
    D_800BCA14 = b;
    D_800BCA18 = k;
    return 0;
}

struct huft;

typedef struct {
    struct huft *tl;
    struct huft *td;
    s32 bl;
    s32 bd;
} FixedTables;

extern FixedTables D_800BCA1C;
extern u16 D_800B038C[];
extern u16 D_800B03CC[];
extern u16 D_800B040C[];
extern u16 D_800B0448[];

s32 func_80051C98(u32 *b, u32 n, u32 s, u16 *d, u16 *e, struct huft **t, s32 *m);
s32 func_80052334(struct huft *t);
s32 func_80050D80(struct huft *tl, struct huft *td, s32 bl, s32 bd);

s32 func_8005130C(void) {
    s32 i;
    u32 l[288];

    if (D_800BCA1C.tl == NULL) {
        for (i = 0; i < 144; i++) {
            l[i] = 8;
        }
        for (; i < 256; i++) {
            l[i] = 9;
        }
        for (; i < 280; i++) {
            l[i] = 7;
        }
        for (; i < 288; i++) {
            l[i] = 8;
        }
        D_800BCA1C.bl = 7;
        if ((i = func_80051C98(l, 288, 257, D_800B038C, D_800B03CC, &D_800BCA1C.tl, &D_800BCA1C.bl)) != 0) {
            D_800BCA1C.tl = NULL;
            return i;
        }
        for (i = 0; i < 30; i++) {
            l[i] = 5;
        }
        D_800BCA1C.bd = 5;
        if ((i = func_80051C98(l, 30, 0, D_800B040C, D_800B0448, &D_800BCA1C.td, &D_800BCA1C.bd)) > 1) {
            func_80052334(D_800BCA1C.tl);
            D_800BCA1C.tl = NULL;
            return i;
        }
    }
    return func_80050D80(D_800BCA1C.tl, D_800BCA1C.td, D_800BCA1C.bl, D_800BCA1C.bd) != 0;
}
