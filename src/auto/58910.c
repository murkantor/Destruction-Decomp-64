#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { Gfx *dl; u8 pad[0x1C]; } DlFrame;
typedef struct { DlFrame frames[10]; u32 depth; u32 max; DlFrame *fp; } DlIter;

#define DL_NEXT(it, g, r, done)                                 \
    {                                                       \
        for (;;) {                                          \
            Gfx *p;                                         \
            Gfx c;                                          \
            u32 w;                                          \
            p = (it).fp[(it).depth].dl++;                   \
            c = *p;                                         \
            w = c.w1;                                       \
            switch (c.w0) {                                 \
            case 0xDE000000: {                              \
                u32 d = (it).depth + 1;                     \
                if (d < (it).max) {                         \
                    (it).depth = d;                         \
                    (it).fp[d].dl = (Gfx *)w;               \
                }                                           \
                break;                                      \
            }                                               \
            case 0xDE010000:                                \
                (it).fp[(it).depth].dl = (Gfx *)w;          \
                break;                                      \
            case 0xDF000000:                                \
                if ((it).depth == 0) {                      \
                    r = NULL;                               \
                    goto done;                              \
                }                                           \
                (it).depth--;                               \
                break;                                      \
            default:                                        \
                r = p;                                      \
                goto done;                                  \
            }                                               \
        }                                                   \
    done:                                                   \
        g = r;                                              \
    }

s32 Gfx_ReplaceCommand(Gfx *dl, u32 max, Gfx match, Gfx repl) {
    DlIter it;
    Gfx *g;
    s32 count = 0;

    it.fp = it.frames;
    it.depth = -1;
    it.max = -1;
    it.depth = 0;
    it.max = max;
    it.fp[0].dl = dl;
    {
        Gfx *r1;
        DL_NEXT(it, g, r1, d1);
    }
    while (g != NULL) {
        if (match.w0 == g->w0 && match.w1 == g->w1) {
            *g = repl;
            count++;
        }
        DL_NEXT(it, g, g, d2);
    }
    return count;
}
