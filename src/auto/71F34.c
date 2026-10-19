#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x18];
    s32 outputRate;
    u8 fxType;
    u8 pad1D[3];
    s32 *params;
} ALSynConfig;

typedef struct {
    s16 fc;
    s16 fgain;
    u8 pad4[4];
    s16 fccoef[16];
    void *fstate;
    s32 first;
} ALLowPass;

typedef struct {
    u8 pad0[0x14];
    void *state;
    u8 pad18[8];
    f32 delta;
    s32 first;
} ALResampler;

typedef struct {
    u32 input;
    u32 output;
    s16 ffcoef;
    s16 fbcoef;
    s16 gain;
    f32 rsinc;
    f32 rsval;
    s32 rsdelta;
    f32 rsgain;
    ALLowPass *lp;
    ALResampler *rs;
} ALDelay;

typedef struct {
    void *source;
    s32 (*handler)();
    s32 (*setParam)();
    s16 inp;
    s16 outp;
    s32 type;
    s16 *base;
    s16 *input;
    u32 length;
    ALDelay *delay;
    u8 section_count;
    u8 pad25[3];
    s32 (*paramHdl)();
} ALFx;

extern s32 D_800B6910[];
extern s32 D_800B6978[];
extern s32 D_800B6A00[];
extern s32 D_800B6A28[];
extern s32 D_800B6A50[];
extern s32 D_800B6A78[];

void alFilterNew(void *, s32, s32 (*)(), s32);
s32 alFxParam();
s32 alFxPull();
s32 alFxParamHdl();
void *alHeapDBAlloc(u8 *, s32, void *, s32, s32);

static inline void _init_lpfilter(ALLowPass *lp) {
    s32 i, temp;
    s16 fc;
    f64 ffc, fcoef;

    temp = lp->fc * 16384;
    fc = temp >> 15;
    lp->fgain = 16384 - fc;
    lp->first = 1;
    for (i = 0; i < 8; i++)
        lp->fccoef[i] = 0;
    lp->fccoef[i++] = fc;
    fcoef = ffc = (f64)fc / 16384;
    for (; i < 16; i++) {
        fcoef *= ffc;
        lp->fccoef[i] = (s16)(fcoef * 16384);
    }
}

void alFxNew(ALFx *r, ALSynConfig *c, void *hp) {
    u16 i, j, k;
    s32 *param = 0;
    ALDelay *d;

    alFilterNew(r, 0, alFxParam, 5);
    r->handler = alFxPull;
    r->paramHdl = alFxParamHdl;

    switch (c->fxType) {
    case 1: param = D_800B6910; break;
    case 2: param = D_800B6978; break;
    case 5: param = D_800B6A00; break;
    case 3: param = D_800B6A28; break;
    case 4: param = D_800B6A50; break;
    case 6: param = c->params; break;
    default: param = D_800B6A78; break;
    }

    j = 0;
    r->section_count = param[j++];
    r->length = param[j++];

    r->delay = alHeapDBAlloc(0, 0, hp, r->section_count, sizeof(ALDelay));
    r->base = alHeapDBAlloc(0, 0, hp, r->length, sizeof(s16));
    r->input = r->base;

    for (k = 0; k < r->length; k++)
        r->base[k] = 0;

    for (i = 0; i < r->section_count; i++) {
        d = &r->delay[i];
        d->input = param[j++];
        d->output = param[j++];
        d->fbcoef = param[j++];
        d->ffcoef = param[j++];
        d->gain = param[j++];

        if (param[j]) {
            d->rsinc = ((((f32)param[j++]) / 1000) * 2.0) / c->outputRate;
            d->rsgain = (((f32)param[j++]) / 173123.404906676) * (d->output - d->input);
            d->rsval = 1.0;
            d->rsdelta = 0.0;
            d->rs = alHeapDBAlloc(0, 0, hp, 1, 0x34);
            d->rs->state = alHeapDBAlloc(0, 0, hp, 1, 0x20);
            d->rs->delta = 0.0;
            d->rs->first = 1;
        } else {
            d->rs = 0;
            j++;
            j++;
        }

        if (param[j]) {
            d->lp = alHeapDBAlloc(0, 0, hp, 1, sizeof(ALLowPass));
            d->lp->fstate = alHeapDBAlloc(0, 0, hp, 1, 8);
            d->lp->fc = param[j++];
            _init_lpfilter(d->lp);
        } else {
            d->lp = 0;
            j++;
        }
    }
}
