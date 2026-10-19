#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Acmd;
typedef s32 (*ALSetParam)(void *, s32, void *);
typedef struct ALFilter_s { struct ALFilter_s *source; void *handler; ALSetParam setParam; s16 inp; s16 outp; s32 type; } ALFilter;
typedef struct ALParam_s { struct ALParam_s *next; s32 delta; s16 type; s16 unity; union { f32 f; s32 i; } data; union { f32 f; s32 i; } moredata; } ALParam;
typedef struct { ALParam *next; s32 delta; s16 type; s16 unity; void *wave; } ALStartParam;
typedef struct { ALParam *next; s32 delta; s16 type; s16 unity; f32 pitch; s16 volume; u8 pan; u8 fxMix; s32 samples; void *wave; } ALStartParamAlt;
typedef struct { u8 pad[0xD8]; s32 offset; } ALVoiceX;
typedef struct { ALParam *next; s32 delta; s16 type; s16 pad; ALVoiceX *pvoice; } ALFreeParam;
typedef struct {
    ALFilter filter;
    void *state;
    s16 pan;
    s16 volume;
    s16 cvolL;
    s16 cvolR;
    s16 dryamt;
    s16 wetamt;
    u16 lratl;
    s16 lratm;
    s16 ltgt;
    u16 rratl;
    s16 rratm;
    s16 rtgt;
    s32 delta;
    s32 segEnd;
    s32 first;
    ALParam *ctrlList;
    ALParam *ctrlTail;
} ALEnvMixer;
typedef struct { u8 drvr[1]; } ALGlobals;

extern ALGlobals *alGlobals;
extern s16 D_800B6AA0[];
extern void __assert(const char *, const char *, s32);
extern Acmd *_pullSubFrame(void *filter, s16 *inp, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p);
extern f32 func_80073384(f32 ivol, s32 samples, s16 ratem, u16 ratel);
extern void _freePVoice(void *drvr, void *pvoice);
extern void __freeParam(ALParam *param);

extern char D_800BC370[];
extern char D_800BC374[];
#define assert(EX) ((EX) ? ((void)0) : __assert(D_800BC370, D_800BC374, __LINE__))

Acmd *func_800726E0(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p) {
    Acmd *ptr = p;
    ALEnvMixer *e = (ALEnvMixer *)filter;
    s16 inp;
    s32 lastOffset;
    s32 thisOffset = sampleOffset;
    s32 samples;
    s16 loutp = 0;
    s32 fVol;
    ALParam *thisParam;

    inp = 0;

    while (e->ctrlList != 0) {
        lastOffset = thisOffset;
        thisOffset = e->ctrlList->delta;
        samples = thisOffset - lastOffset;
        if (samples > outCount)
            break;

#line 104
        assert(samples >= 0);
        assert(samples <= 160);

        switch (e->ctrlList->type) {
        case 13: {
            ALStartParamAlt *param = (ALStartParamAlt *)e->ctrlList;
            ALFilter *f = (ALFilter *)e;
            s32 tmp;

            if (param->unity) {
                (*e->filter.setParam)(&e->filter, 8, 0);
            }
            (*e->filter.setParam)(&e->filter, 5, param->wave);
            (*e->filter.setParam)(&e->filter, 9, 0);

            e->first = 1;
            e->delta = 0;
            e->segEnd = param->samples;

            tmp = ((s32)param->volume * (s32)param->volume) >> 15;
            e->volume = (s16)tmp;
            e->pan = param->pan;
            e->dryamt = D_800B6AA0[param->fxMix];
            e->wetamt = D_800B6AA0[127 - param->fxMix];

            if (param->samples) {
                e->cvolL = 1;
                e->cvolR = 1;
            } else {
                e->cvolL = (e->volume * D_800B6AA0[e->pan]) >> 15;
                e->cvolR = (e->volume * D_800B6AA0[127 - e->pan]) >> 15;
            }

            if (f->source) {
                union { f32 f; s32 i; } data;
                data.f = param->pitch;
                (*f->source->setParam)(f->source, 7, (void *)data.i);
            }
        } break;

        case 16:
        case 12:
        case 11:
            ptr = _pullSubFrame(e, &inp, &loutp, samples, sampleOffset, ptr);

            if (e->delta >= e->segEnd) {
                e->ltgt = (e->volume * D_800B6AA0[e->pan]) >> 15;
                e->rtgt = (e->volume * D_800B6AA0[127 - e->pan]) >> 15;
                e->delta = e->segEnd;
                e->cvolL = e->ltgt;
                e->cvolR = e->rtgt;
            } else {
                e->cvolL = func_80073384(e->cvolL, e->delta, e->lratm, e->lratl);
                e->cvolR = func_80073384(e->cvolR, e->delta, e->rratm, e->rratl);
            }

            if (e->cvolL == 0) e->cvolL = 1;
            if (e->cvolR == 0) e->cvolR = 1;

            if (e->ctrlList->type == 12)
                e->pan = (s16)e->ctrlList->data.i;

            if (e->ctrlList->type == 11) {
                e->delta = 0;
                fVol = (e->ctrlList->data.i);
                fVol = (fVol * fVol) >> 15;
                e->volume = (s16)fVol;
                e->segEnd = e->ctrlList->moredata.i;
            }

            if (e->ctrlList->type == 16) {
                e->dryamt = D_800B6AA0[e->ctrlList->data.i];
                e->wetamt = D_800B6AA0[127 - e->ctrlList->data.i];
            }

            e->first = 1;
            break;

        case 14: {
            ALStartParam *p = (ALStartParam *)e->ctrlList;
            if (p->unity) {
                (*e->filter.setParam)(&e->filter, 8, 0);
            }
            (*e->filter.setParam)(&e->filter, 5, p->wave);
            (*e->filter.setParam)(&e->filter, 9, 0);
        } break;

        case 15:
            ptr = _pullSubFrame(e, &inp, &loutp, samples, sampleOffset, ptr);
            (*e->filter.setParam)(&e->filter, 4, 0);
            break;

        case 0: {
            void *drvr = &alGlobals->drvr;
            ALFreeParam *param = (ALFreeParam *)e->ctrlList;
            param->pvoice->offset = 0;
            _freePVoice(drvr, param->pvoice);
        } break;

        default:
            ptr = _pullSubFrame(e, &inp, &loutp, samples, sampleOffset, ptr);
            (*e->filter.setParam)(&e->filter, e->ctrlList->type, (void *)e->ctrlList->data.i);
            break;
        }
        loutp += (samples << 1);
        outCount -= samples;

        thisParam = e->ctrlList;
        e->ctrlList = e->ctrlList->next;
        if (e->ctrlList == 0)
            e->ctrlTail = 0;

        __freeParam(thisParam);
    }

    ptr = _pullSubFrame(e, &inp, &loutp, outCount, sampleOffset, ptr);

    if (e->delta > e->segEnd)
        e->delta = e->segEnd;

    return ptr;
}
