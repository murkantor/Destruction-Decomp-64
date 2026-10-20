#include "common.h"
#include "m2c.h"

extern u32 D_800BCA10;
extern u32 D_800BCA14;
extern u32 D_800BCA18;
extern u32 D_800BCA2C;
extern s32 D_800BCA30;

s32 Inflate_GetByte(void);
s32 inflate_dynamic(void);
s32 Inflate_Stored(void);
s32 inflate_fixed(void);
void Inflate_FlushWindow(s32, u32);

#define NEEDBITS(n)                  \
    while (k < (n)) {                \
        s32 c = Inflate_GetByte();     \
        if (c == 0x100) {            \
            return 1;                \
        }                            \
        b |= (u32)c << k;            \
        k += 8;                      \
    }
#define DUMPBITS(n) \
    {               \
        b >>= (n);  \
        k -= (n);   \
    }

static inline s32 InflateBlock(s32 *e) {
    u32 t;
    u32 b;
    u32 k;

    b = D_800BCA14;
    k = D_800BCA18;
    NEEDBITS(1);
    *e = (s32)b & 1;
    DUMPBITS(1);
    NEEDBITS(2);
    t = b & 3;
    DUMPBITS(2);
    D_800BCA14 = b;
    D_800BCA18 = k;
    if (t == 2) {
        return inflate_dynamic();
    }
    if (t == 0) {
        return Inflate_Stored();
    }
    if (t == 1) {
        return inflate_fixed();
    }
    return 2;
}

s32 inflate(void) {
    s32 e;
    s32 r;
    u32 h;

    D_800BCA10 = 0;
    D_800BCA18 = 0;
    D_800BCA14 = 0;
    h = 0;
    do {
        D_800BCA2C = 0;
        if ((r = InflateBlock(&e)) != 0) {
            return r;
        }
        if (D_800BCA2C > h) {
            h = D_800BCA2C;
        }
    } while (!e);
    Inflate_FlushWindow(D_800BCA30, D_800BCA10);
    return 0;
}
