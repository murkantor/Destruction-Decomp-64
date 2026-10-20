typedef struct {
    u8 pad[0x5C];
    s32 x5C;
    s32 x60;
    s32 x64;
    u8 pad68[0x71 - 0x68];
    u8 x71;
} Disp;

typedef struct {
    u8 pad[0x8C];
    Disp *x8C;
    u8 *x90;
} Obj;

extern u8 D_800B0C90[][11][47][16];
void osWritebackDCacheAll(void);

void Font_BlitString(Obj *o, s32 x, s32 y, u8 *str) {
    Disp *d = o->x8C;
    s32 bpp = d->x71;
    u8 *base;
    s32 off;
    s32 row, k;
    u8 *dst;
    u8 *src;
    s32 c, q, r;
    s32 xx, yy, pp;
    u8 *b0;
    b0 = o->x90; pp = d->x64; xx = (s32)(x * 0.003125f * d->x5C) * bpp; yy = (s32)(y * (1.0f / 240.0f) * d->x60); base = b0 + (yy * pp + xx);

    for (off = 0; str[off] != 0; off++) {
        c = str[off] - 0x20;
        if (c > 0) {
            c = str[off] - 0x21;
            r = c % 47; q = c / 47;
            src = D_800B0C90[q][0][r];
            dst = base + off * 8 * bpp;
            for (row = 0; row < 11; row++) {
                for (k = 0; k < 16; k++) {
                    dst[k] = src[k];
                }
                dst += o->x8C->x64;
                src += 47 * 16;
            }
        }
    }
    osWritebackDCacheAll();
}
/* agent55: score 2 (one addu). The original computes (b0 + xx) + yy*pp with b0 in v1; every form that adds b0 + xx first (168 statement orders x base forms, decl orders, s32/u32 types, &b0[..], b0 += xx) swaps v0/v1 throughout (score 10-14). */
