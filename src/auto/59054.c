#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

s32 Gfx_IsCommandKind(Gfx, s32);

Gfx *Gfx_CopyDisplayList(Gfx *dst, Gfx *src, s32 depth, s32 arg3, s32 *count) {
    s32 pops = 0;
    Gfx cmd;
    u32 w1;

    while (*count != 0) {
        cmd = *src++;
        w1 = cmd.w1;
        switch (cmd.w0) {
            case 0xDE000000:
                if (depth != 0) {
                    dst = Gfx_CopyDisplayList(dst, (Gfx *)w1, depth - 1, arg3, count);
                }
                break;
            case 0xDE010000:
                src = (Gfx *)w1;
                break;
            case 0xDF000000:
                goto end;
            case 0xDA000000:
                pops += 2;
            case 0xD8000000:
                pops--;
            default:
                if (Gfx_IsCommandKind(cmd, arg3)) {
                    *dst++ = cmd;
                    (*count)--;
                }
                break;
        }
    }
end:
    while (pops != 0) {
        pops--;
        {
            Gfx *g = dst++;
            g->w0 = 0xD8380002;
            g->w1 = 0x40;
        }
    }
    return dst;
}
