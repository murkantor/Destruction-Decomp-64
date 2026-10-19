#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Acmd;
typedef struct {
    s16 fc;
    s16 fgain;
    union {
        s16 fccoef[16];
        s64 force_aligned;
    } fcvec;
    s16 *fstate;
    s32 first;
} ALLowPass;
u32 osVirtualToPhysical(void *);
#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define aSetBuffer(pkt, f, i, o, c) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = (_SHIFTL(8, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(i, 0, 16)); \
        _a->w1 = _SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16); \
    }
#define aLoadADPCM(pkt, c, d) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(11, 24, 8) | _SHIFTL(c, 0, 24); \
        _a->w1 = (u32)d; \
    }
#define aPoleFilter(pkt, f, g, s) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = (_SHIFTL(14, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(g, 0, 16)); \
        _a->w1 = (u32)(s); \
    }

Acmd *_filterBuffer(ALLowPass *lp, s32 buff, s32 count, Acmd *p) {
    Acmd *ptr = p;

    aSetBuffer(ptr++, 0, buff, buff, count << 1);
    aLoadADPCM(ptr++, 32, osVirtualToPhysical(lp->fcvec.fccoef));
    aPoleFilter(ptr++, lp->first, lp->fgain, osVirtualToPhysical(lp->fstate));
    lp->first = 0;
    return ptr;
}
