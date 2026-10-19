#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { Gfx *ptr; u8 pad[0x1C]; } DLFrame;
typedef struct { u32 depth; u32 max; DLFrame *frames; } DLStack;

Gfx *Gfx_NextCmd(DLStack *s) {
    Gfx *g;
    Gfx cmd;
    u32 op;
    Gfx *next;

    while (1) {
        g = s->frames[s->depth].ptr++;
        cmd = *g;
        op = cmd.w0;
        next = (Gfx *)cmd.w1;
        switch (op) {
        case 0xDE000000:
            if (s->depth + 1 < s->max) {
                s->depth++;
                s->frames[s->depth].ptr = next;
            }
            break;
        case 0xDE010000:
            s->frames[s->depth].ptr = next;
            break;
        case 0xDF000000:
            if (s->depth == 0) {
                return NULL;
            }
            s->depth--;
            break;
        default:
            return g;
        }
    }
}
