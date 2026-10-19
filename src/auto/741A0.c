#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Acmd;
typedef Acmd *(*ALCmdHandler)(void *, s16 *, s32, s32, Acmd *);
typedef struct ALFilter_s {
    struct ALFilter_s *source;
    ALCmdHandler handler;
    void *setParam;
    s16 inp;
    s16 outp;
    s32 type;
} ALFilter;
typedef struct {
    ALFilter filter;
    void *state;
    f32 ratio;
    s32 upitch;
    f32 delta;
    s32 first;
} ALResampler;

u32 osVirtualToPhysical(void *);

#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))

Acmd *alResamplePull(void *filter, s16 *outp, s32 outCnt, s32 sampleOffset, Acmd *p) {
    ALResampler *f = (ALResampler *)filter;
    Acmd *ptr = p;
    s16 inp;
    s32 inCount;
    ALFilter *source = f->filter.source;
    s32 incr;
    f32 finCount;

    inp = 0x140;

    if (!outCnt)
        return ptr;

    if (f->upitch) {
        ptr = (*source->handler)(source, &inp, outCnt, sampleOffset, p);
        {
            Acmd *_a = ptr++;
            _a->w0 = _SHIFTL(10, 24, 8) | _SHIFTL(inp, 0, 24);
            _a->w1 = _SHIFTL(*outp, 16, 16) | _SHIFTL(outCnt << 1, 0, 16);
        }
    } else {
        if (f->ratio > 1.99996)
            f->ratio = 1.99996;

        f->ratio = (s32)(f->ratio * 32768.0f);
        f->ratio = f->ratio / 32768.0f;

        finCount = f->delta + (f->ratio * (f32)outCnt);
        inCount = (s32)finCount;
        f->delta = finCount - (f32)inCount;

        ptr = (*source->handler)(source, &inp, inCount, sampleOffset, p);

        incr = (s32)(f->ratio * 32768.0f);
        {
            Acmd *_a = ptr++;
            _a->w0 = _SHIFTL(8, 24, 8) | _SHIFTL(0, 16, 8) | _SHIFTL(inp, 0, 16);
            _a->w1 = _SHIFTL(*outp, 16, 16) | _SHIFTL(outCnt << 1, 0, 16);
        }
        {
            Acmd *_a = ptr++;
            _a->w0 = _SHIFTL(5, 24, 8) | _SHIFTL(f->first, 16, 8) | _SHIFTL(incr, 0, 16);
            _a->w1 = (u32)osVirtualToPhysical(f->state);
        }
        f->first = 0;
    }
    return ptr;
}
