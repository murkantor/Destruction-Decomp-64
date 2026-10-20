#include "common.h"
#include "m2c.h"

typedef union {
    struct {
        u32 w0;
        u32 w1;
    } words;
    long long force_union_align;
} Acmd;

typedef s32 (*ALDMAproc)(s32 addr, s32 len, void *state);

typedef struct {
    u8 *base;
    s32 len;
} ALWaveTable;

typedef struct {
    u32 start;
    u32 end;
    u32 count;
} ALRawLoop;

typedef struct {
    void *source;
    void *handler;
    void *setParam;
    s16 inp;
    s16 outp;
    s32 type;
} ALFilter;

typedef struct {
    ALFilter filter;
    void *state;
    void *lstate;
    ALRawLoop loop;
    ALWaveTable *table;
    s32 bookSize;
    ALDMAproc dma;
    void *dmaState;
    s32 sample;
    s32 lastsam;
    s32 first;
    s32 memin;
} ALLoadFilter;

#define _SHIFTL(v, s, w) ((u32)(((u32)(v) & ((1 << (w)) - 1)) << (s)))
#define A_CLEARBUFF 2
#define A_LOADBUFF 4
#define A_SETBUFF 8
#define A_DMEMMOVE 10

#define aSetBuffer(pkt, f, i, o, c) { \
    Acmd *_a = (Acmd *)pkt; \
    _a->words.w0 = (_SHIFTL(A_SETBUFF, 24, 8) | _SHIFTL(f, 16, 8) | _SHIFTL(i, 0, 16)); \
    _a->words.w1 = (_SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16)); }

#define aLoadBuffer(pkt, s) { \
    Acmd *_a = (Acmd *)pkt; \
    _a->words.w0 = _SHIFTL(A_LOADBUFF, 24, 8); \
    _a->words.w1 = (u32)(s); }

#define aDMEMMove(pkt, i, o, c) { \
    Acmd *_a = (Acmd *)pkt; \
    _a->words.w0 = _SHIFTL(A_DMEMMOVE, 24, 8) | _SHIFTL(i, 0, 24); \
    _a->words.w1 = _SHIFTL(o, 16, 16) | _SHIFTL(c, 0, 16); }

#define aClearBuffer(pkt, d, c) { \
    Acmd *_a = (Acmd *)pkt; \
    _a->words.w0 = _SHIFTL(A_CLEARBUFF, 24, 8) | _SHIFTL(d, 0, 24); \
    _a->words.w1 = (u32)(c); }

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

Acmd *alRaw16Pull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p) {
    Acmd *ptr = p;
    ALLoadFilter *f = (ALLoadFilter *)filter;
    s32 nbytes;
    s32 dramLoc;
    s32 dramAlign;
    s32 dmemAlign;
    s32 overFlow;
    s32 startZero;
    s32 nSam;
    s32 op;

    if (outCount == 0)
        return ptr;

    if ((outCount + f->sample > f->loop.end) && (f->loop.count != 0)) {
        nSam = f->loop.end - f->sample;
        nbytes = nSam << 1;
        if (nSam > 0) {
            dramLoc = (f->dma)(f->memin, nbytes, f->dmaState);
            dramAlign = dramLoc & 0x7;
            nbytes += dramAlign;
            aSetBuffer(ptr++, 0, *outp, 0, nbytes + 8 - (nbytes & 0x7));
            aLoadBuffer(ptr++, dramLoc - dramAlign);
        } else
            dramAlign = 0;

        *outp += dramAlign;

        f->memin = (s32)f->table->base + (f->loop.start << 1);
        f->sample = f->loop.start;
        op = *outp;

        while (outCount > nSam) {
            op += (nSam << 1);
            outCount -= nSam;
            if ((f->loop.count != -1) && (f->loop.count != 0))
                f->loop.count--;

            nSam = MIN(outCount, f->loop.end - f->loop.start);
            nbytes = nSam << 1;

            dramLoc = (f->dma)(f->memin, nbytes, f->dmaState);
            dramAlign = dramLoc & 0x7;
            nbytes += dramAlign;
            if ((op & 0x7))
                dmemAlign = 8 - (op & 0x7);
            else
                dmemAlign = 0;

            aSetBuffer(ptr++, 0, op + dmemAlign, 0, nbytes + 8 - (nbytes & 0x7));
            aLoadBuffer(ptr++, dramLoc - dramAlign);

            if (dramAlign || dmemAlign) {
                aDMEMMove(ptr++, op + dramAlign + dmemAlign, op, nSam << 1);
            }
        }
        f->sample += outCount;
        f->memin += (outCount << 1);
        return ptr;
    } else {
        nbytes = outCount << 1;
        overFlow = f->memin + nbytes - ((s32)f->table->base + f->table->len);

        if (overFlow < 0)
            overFlow = 0;

        if (overFlow > nbytes)
            overFlow = nbytes;

        if (overFlow < nbytes) {
            if (outCount > 0) {
                nbytes -= overFlow;
                dramLoc = (f->dma)(f->memin, nbytes, f->dmaState);
                dramAlign = dramLoc & 0x7;
                nbytes += dramAlign;
                aSetBuffer(ptr++, 0, *outp, 0, nbytes + 8 - (nbytes & 0x7));
                aLoadBuffer(ptr++, dramLoc - dramAlign);
            } else
                dramAlign = 0;
            *outp += dramAlign;

            f->sample += outCount;
            f->memin += outCount << 1;
        } else {
            f->memin += outCount << 1;
        }

        if (overFlow) {
            startZero = (outCount << 1) - overFlow;
            if (startZero < 0)
                startZero = 0;
            aClearBuffer(ptr++, startZero + *outp, overFlow);
        }
    }

    return ptr;
}
