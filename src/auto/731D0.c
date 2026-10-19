#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Acmd;
typedef struct ALFilter_s {
    struct ALFilter_s *source;
    Acmd *(*handler)(void *, s16 *, s32, s32, Acmd *);
    s32 (*setParam)(void *, s32, void *);
    s16 inp;
    s16 outp;
    s32 type;
} ALFilter;
#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define aClearBuffer(pkt, d, c) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(2, 24, 8) | _SHIFTL(d, 0, 24); \
        _a->w1 = (u32)(c); \
    }
#define aSetBuffer(pkt, f, i, o, c) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = (_SHIFTL(8, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(i, 0, 16)); \
        _a->w1 = _SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16); \
    }
#define aInterleave(pkt, l, r) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(13, 24, 8); \
        _a->w1 = _SHIFTL(l, 16, 16) | _SHIFTL(r, 0, 16); \
    }
#define aSaveBuffer(pkt, s) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(6, 24, 8); \
        _a->w1 = (u32)(s); \
    }

typedef struct {
    ALFilter filter;
    s32 sourceCount;
    s32 maxSources;
    ALFilter **sources;
} ALAuxBus;

Acmd *alAuxBusPull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p) {
    Acmd *ptr = p;
    ALAuxBus *m = (ALAuxBus *)filter;
    ALFilter **sources = m->sources;
    s32 i;

    aClearBuffer(ptr++, 0x6C0, outCount << 1);
    aClearBuffer(ptr++, 0x800, outCount << 1);
    for (i = 0; i < m->sourceCount; i++) {
        ptr = (sources[i]->handler)(sources[i], outp, outCount, sampleOffset, ptr);
    }
    return ptr;
}
