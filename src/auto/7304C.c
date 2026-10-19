#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Acmd;

typedef s32 (*ALDMAproc)(s32 addr, s32 len, void *state);

typedef struct {
    u8 pad0[0x14];
    void *state;
    void *lstate;
    u8 pad1C[0x30 - 0x1C];
    ALDMAproc dma;
    void *dmaState;
    u8 pad38[0x40 - 0x38];
    s32 first;
    s32 memin;
} ALLoadFilter;

#define _SHIFTL(v, s, w) ((unsigned int)(((unsigned int)(v) & ((0x01 << (w)) - 1)) << (s)))
#define K0_TO_PHYS(x) ((u32)(x) & 0x1FFFFFFF)

#define aSetBuffer(pkt, f, i, o, c) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = (_SHIFTL(8, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(i, 0, 16)); \
        _a->w1 = _SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16); \
    }
#define aLoadBuffer(pkt, s) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(4, 24, 8); \
        _a->w1 = (unsigned int)(s); \
    }
#define aSetLoop(pkt, a) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(15, 24, 8); \
        _a->w1 = (unsigned int)(a); \
    }
#define aADPCMdec(pkt, f, s) \
    { \
        Acmd *_a = (Acmd *)pkt; \
        _a->w0 = _SHIFTL(1, 24, 8) | _SHIFTL(f, 16, 8); \
        _a->w1 = (unsigned int)(s); \
    }

Acmd *_decodeChunk(Acmd *ptr, ALLoadFilter *f, s32 tsam, s32 nbytes, s16 outp, s16 inp, u32 flags) {
    s32 dramAlign;
    s32 dramLoc;

    if (nbytes > 0) {
        dramLoc = (f->dma)(f->memin, nbytes, f->dmaState);
        dramAlign = dramLoc & 0x7;
        nbytes += dramAlign;
        aSetBuffer(ptr++, 0, inp, 0, nbytes + 8 - (nbytes & 0x7));
        aLoadBuffer(ptr++, dramLoc - dramAlign);
    } else {
        dramAlign = 0;
    }
    if (flags & 2) {
        aSetLoop(ptr++, K0_TO_PHYS(f->lstate));
    }
    aSetBuffer(ptr++, 0, inp + dramAlign, outp, tsam << 1);
    aADPCMdec(ptr++, flags, K0_TO_PHYS(f->state));
    f->first = 0;
    return ptr;
}
