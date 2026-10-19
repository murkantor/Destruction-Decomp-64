typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct {
    u8 pad0[4];
    s32 x4;
    s32 x8;
    u8 *xC;
    u8 *x10;
    u8 *x14;
} Font;

extern char D_800BBB6C[];
extern char D_800BBB70[];
void __assert(const char *, const char *, int);

#define GFX2(g,a,b) { Gfx *_g = (g)++; _g->w0 = (a); _g->w1 = (u32)(b); }

Gfx *Font_DrawString(Font *font, s32 x0, s32 y0, u8 *str, Gfx *g) {
    s32 x;
    s32 y;
    u8 c;
    u8 pal;
    u8 ch;

    if (font == NULL) {
        __assert(D_800BBB6C, D_800BBB70, 0x78);
    }
    if (str == NULL) {
        __assert(D_800BBB6C, D_800BBB70, 0x79);
    }
    x = x0;
    y = y0;
    GFX2(g, 0xE7000000, 0);
    GFX2(g, 0xE2001E01, 1);
    GFX2(g, 0xF9000000, 0x80);
    GFX2(g, 0xE3000C00, 0);
    GFX2(g, 0xE3000D01, 0);
    GFX2(g, 0xE3000F00, 0);
    GFX2(g, 0xE3001201, 0);
    GFX2(g, 0xE3001402, 0xC00);
    GFX2(g, 0xE3001001, 0x8000);
    GFX2(g, 0xD9000000, 0);
    GFX2(g, 0xE3000A01, 0);
    GFX2(g, 0xE200001C, 0x3466048);
    GFX2(g, 0xFCFFFFFF, 0xFFFCF279);
    pal = 0;
    while ((c = *str++) != 0) {
        switch (c) {
        case 1:
            pal = *str++;
            break;
        case 10:
            y = y + 1 + font->x8;
            x = x0;
            break;
        default:
            if (c >= 0x20) {
                ch = c - 0x20;
                if (ch != 0) {
                    GFX2(g, 0xFD480000 | (((font->x4 >> 1) - 1) & 0xFFF), font->x14 + font->x4 / 2 * font->x8 * font->x10[ch]);
                    GFX2(g, 0xF5480200, 0x07000000);
                    GFX2(g, 0xE6000000, 0);
                    GFX2(g, 0xF4000000, 0x07010020);
                    GFX2(g, 0xE7000000, 0);
                    GFX2(g, 0xF5400200, (pal & 0xF) << 20);
                    GFX2(g, 0xF2000000, 0x00020020);
                    GFX2(g, 0xE4000000 | ((((x + font->x4) * 4) & 0xFFF) << 12) | (((y + font->x8) * 4) & 0xFFF), (((x * 4) & 0xFFF) << 12) | ((y * 4) & 0xFFF));
                    GFX2(g, 0xE1000000, 0);
                    GFX2(g, 0xF1000000, 0x04000400);
                }
                x += font->xC[ch];
            }
            break;
        }
    }
    return g;
}
/* agent45: not improved; header GFX stores spill pattern differs (ours frame 0x80 vs 0x78). */
