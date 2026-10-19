#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { Gfx *ptr; u8 pad[0x1C]; } DLFrame;
typedef struct { u32 depth; u32 max; DLFrame *stack; } DLWalker;

Gfx *Gfx_WalkToCommand(DLWalker *w, Gfx *dl, u32 max) {
    Gfx *g;
    Gfx cmd;
    Gfx *next;

    w->depth = 0;
    w->max = max;
    w->stack[w->depth].ptr = dl;
    for (;;) {
        g = w->stack[w->depth].ptr++;
        cmd = *g;
        next = (Gfx *)cmd.w1;
        switch (cmd.w0) {
            case 0xDE000000:
                if (w->depth + 1 < w->max) {
                    w->depth++;
                    w->stack[w->depth].ptr = next;
                }
                break;
            case 0xDE010000:
                w->stack[w->depth].ptr = next;
                break;
            case 0xDF000000:
                if (w->depth == 0) {
                    return NULL;
                }
                w->depth--;
                break;
            default:
                return g;
        }
    }
}
