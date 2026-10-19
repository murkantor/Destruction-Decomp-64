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
    s32 dramout;
    s32 first;
} ALSave;
extern char D_800BC500[];
extern char D_800BC504[];
void __assert(const char *, const char *, int);

Acmd *alSavePull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p) {
    Acmd *ptr = p;
    ALSave *f = (ALSave *)filter;
    ALFilter *source = f->filter.source;

    if (!source) {
        __assert(D_800BC500, D_800BC504, 34);
    }
    ptr = (*source->handler)(source, outp, outCount, sampleOffset, ptr);
    aSetBuffer(ptr++, 0, 0, 0, outCount << 1);
    aInterleave(ptr++, 0x440, 0x580);
    aSetBuffer(ptr++, 0, 0, 0, outCount << 2);
    aSaveBuffer(ptr++, f->dramout);
    return ptr;
}
