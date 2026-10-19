#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { Gfx *ptr; u8 pad[0x1C]; } DlFrame;
typedef struct { s32 depth; u32 max; DlFrame *stack; } DlWalker;
s32 Gfx_IsCommandKind(Gfx g, s32 kind);

static inline Gfx *DlWalker_Next(DlWalker *w) {
    Gfx *result;
    for (;;) {
        Gfx *g = w->stack[w->depth].ptr++;
        Gfx cmd = *g;
        u32 op = cmd.w0;
        u32 arg = cmd.w1;
        switch (op) {
        case 0xDE000000: {
            s32 d = w->depth + 1;
            if ((u32)d < w->max) {
                w->depth = d;
                w->stack[d].ptr = (Gfx *)arg;
            }
            break;
        }
        case 0xDE010000:
            w->stack[w->depth].ptr = (Gfx *)arg;
            break;
        case 0xDF000000:
            if (w->depth == 0) {
                result = NULL;
                goto out;
            }
            w->depth--;
            break;
        default:
            result = g;
            goto out;
        }
    }
out:
    return result;
}

static inline Gfx *DlWalker_Next2(DlWalker *w) {
    for (;;) {
        Gfx *g = w->stack[w->depth].ptr++;
        Gfx cmd = *g;
        u32 op = cmd.w0;
        u32 arg = cmd.w1;
        switch (op) {
        case 0xDE000000: {
            s32 d = w->depth + 1;
            if ((u32)d < w->max) {
                w->depth = d;
                w->stack[d].ptr = (Gfx *)arg;
            }
            break;
        }
        case 0xDE010000:
            w->stack[w->depth].ptr = (Gfx *)arg;
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

s32 func_8005822C(Gfx *dl, u32 maxDepth, s32 kind) {
    DlFrame frames[10];
    DlWalker w;
    Gfx *g;
    s32 count = 0;

    w.depth = -1;
    w.max = -1;
    w.stack = frames;
    w.depth = 0;
    w.max = maxDepth;
    frames[0].ptr = dl;
    if ((g = DlWalker_Next(&w)) != NULL) {
        do {
            count += Gfx_IsCommandKind(*g, kind);
        } while ((g = DlWalker_Next2(&w)) != NULL);
    }
    return count;
}
