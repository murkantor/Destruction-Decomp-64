typedef struct {
    u32 w0;
    u32 w1;
} Gfx;
extern u8 D_801301F9;
extern u8 D_B7560[];
extern u8 D_7BCC0[];

Gfx *func_80060320(Gfx *g) {
    if (D_801301F9) {
        {
            Gfx *_g = g++;
            _g->w0 = 0xE1000000;
            _g->w1 = (u32)D_B7560;
        }
        {
            Gfx *_g = g++;
            _g->w0 = 0xDD0007FF;
            _g->w1 = (u32)D_7BCC0;
        }
    }
    return g;
}
