#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
void *memset(void *, s32, u32);
s32 Gfx_IsCommandKind(Gfx, s32);

s32 func_80057F2C(Gfx *g, s32 depth, s32 arg) {
    s32 count = 0;
    Gfx cmd;
    Gfx nop;
    u32 w1;
    Gfx *next;

    while (1) {
        cmd = *g++; w1 = cmd.w1; next = (Gfx *)w1;
        switch (cmd.w0) {
            case 0xDE000000:
                if (depth != 0) {
                    count += func_80057F2C((Gfx *)w1, depth - 1, arg);
                }
                break;
            case 0xDE010000:
                g = next;
                break;
            case 0xDF000000:
                return count;
            default:
                memset(&nop, 0, 8);
                nop.w0 = 0;
                if (Gfx_IsCommandKind(cmd, arg)) {
                    g[-1] = nop;
                    count++;
                }
                break;
        }
    }
}
