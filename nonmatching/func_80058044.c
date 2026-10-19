typedef struct { u32 w0; u32 w1; } Gfx;

extern u32 D_800BD20C;
extern u32 D_800BD210;
extern u32 D_800BD214;
extern u32 D_800BD218;
void *memset(void *, s32, u32);

s32 func_80058044(Gfx *dl, s32 depth) {
    s32 count = 0;
    Gfx g;
    Gfx nop;
    u32 w0;
    u32 w1;

    for (;;) {
        g = *dl++;
        w1 = g.w1;
        w0 = g.w0;
        switch (w0) {
        case 0xDE000000:
            if (depth != 0) {
                count += func_80058044((Gfx *)w1, depth - 1);
            }
            continue;
        case 0xDE010000:
            dl = (Gfx *)w1;
            continue;
        case 0xDF000000:
            return count;
        }
        memset(&nop, 0, 8);
        nop.w0 = 0;
        if ((w0 >> 24) == 0xFD) {
            if (g.w0 == D_800BD20C && g.w1 == D_800BD210) {
                dl[-1] = nop;
                count++;
            } else {
                D_800BD214 = 0;
                D_800BD218 = 0;
                D_800BD20C = g.w0;
                D_800BD210 = g.w1;
            }
        }
        if ((w0 >> 24) == 0xF4) {
            if (g.w0 == D_800BD214 && g.w1 == D_800BD218) {
                dl[-1] = nop;
                count++;
            } else {
                D_800BD214 = g.w0;
                D_800BD218 = g.w1;
            }
        }
    }
}
