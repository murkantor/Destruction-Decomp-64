#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Acmd;
typedef struct { u32 start; u32 end; u32 count; } ALRawLoop;
typedef struct { s32 order; s32 npredictors; s16 book[1]; } ALADPCMBook;
typedef struct {
    u8 *base;
    s32 len;
    u8 type;
    u8 flags;
    u8 pad[2];
    void *loop;
    ALADPCMBook *book;
} ALWaveTable;
typedef struct {
    u8 filter[0x14];
    void *state;
    void *lstate;
    ALRawLoop loop;
    ALWaveTable *table;
    s32 bookSize;
    void *dma;
    void *dmaState;
    s32 sample;
    s32 lastsam;
    s32 first;
    s32 memin;
} ALLoadFilter;

Acmd *_decodeChunk(Acmd *cmd, ALLoadFilter *f, s32 tsam, s32 nbytes, s16 outp, s16 inp, u32 flags);

Acmd *alAdpcmPull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p)
{
    s16 inp;
    Acmd *ptr = p;
    s32 tsam;
    s32 nframes;
    s32 nbytes;
    s32 overFlow;
    s32 startZero;
    s32 nOver;
    s32 nSam;
    s32 op;
    s32 nLeft;
    s32 bEnd;
    s32 decoded = 0;
    s32 looped = 0;
    ALLoadFilter *f = (ALLoadFilter *)filter;

    if (outCount == 0)
        return ptr;

    { Acmd *_a = ptr++; _a->w0 = (0x0B << 24) | (f->bookSize & 0xFFFFFF); _a->w1 = (u32)f->table->book->book & 0x1FFFFFFF; }

    inp = 0;
    looped = (outCount + f->sample > f->loop.end) && (f->loop.count != 0);
    if (looped)
        nSam = f->loop.end - f->sample;
    else
        nSam = outCount;

    if (f->lastsam)
        nLeft = 16 - f->lastsam;
    else
        nLeft = 0;
    tsam = nSam - nLeft;
    if (tsam < 0) tsam = 0;

    nframes = (tsam + 16 - 1) >> 4;
    nbytes = nframes * 9;

    if (looped) {
        ptr = _decodeChunk(ptr, f, tsam, nbytes, *outp, inp, f->first);

        if (f->lastsam)
            *outp += (f->lastsam << 1);
        else
            *outp += (16 << 1);

        f->lastsam = f->loop.start & 0xf;
        f->memin = (s32)f->table->base + 9 * ((s32)(f->loop.start >> 4) + 1);
        f->sample = f->loop.start;

        bEnd = *outp;
        while (outCount > nSam) {
            outCount -= nSam;

            op = (bEnd + ((nframes + 1) << 5)) & ~0x1f;

            bEnd += nSam << 1;

            if (f->loop.count != -1 && f->loop.count != 0)
                f->loop.count--;

            nSam = (outCount < f->loop.end - f->loop.start) ? outCount : f->loop.end - f->loop.start;

            tsam = nSam - 16 + f->lastsam;
            if (tsam < 0) tsam = 0;
            nframes = (tsam + 16 - 1) >> 4;
            nbytes = nframes * 9;
            ptr = _decodeChunk(ptr, f, tsam, nbytes, op, inp, f->first | 2);
            { Acmd *_a = ptr++; _a->w0 = (0x0A << 24) | ((op + (f->lastsam << 1)) & 0xFFFFFF); _a->w1 = (bEnd << 16) | ((nSam << 1) & 0xFFFF); }
        }

        f->lastsam = (outCount + f->lastsam) & 0xf;
        f->sample += outCount;
        f->memin += 9 * nframes;
        return ptr;
    }

    nSam = nframes << 4;

    overFlow = f->memin + nbytes - ((s32)f->table->base + f->table->len);

    if (overFlow < 0)
        overFlow = 0;
    nOver = (overFlow / 9) << 4;
    if (nOver > nSam + nLeft)
        nOver = nSam + nLeft;

    nbytes -= overFlow;

    if ((nOver - (nOver & 0xf)) < outCount) {
        decoded = 1;
        ptr = _decodeChunk(ptr, f, nSam - nOver, nbytes, *outp, inp, f->first);

        if (f->lastsam)
            *outp += (f->lastsam << 1);
        else
            *outp += (16 << 1);

        f->lastsam = (outCount + f->lastsam) & 0xf;
        f->sample += outCount;
        f->memin += 9 * nframes;
    } else {
        f->lastsam = 0;
        f->memin += 9 * nframes;
    }

    if (nOver) {
        f->lastsam = 0;
        if (decoded)
            startZero = (nLeft + nSam - nOver) << 1;
        else
            startZero = 0;
        { Acmd *_a = ptr++; _a->w0 = (0x02 << 24) | ((startZero + *outp) & 0xFFFFFF); _a->w1 = nOver << 1; }
    }

    return ptr;
}
