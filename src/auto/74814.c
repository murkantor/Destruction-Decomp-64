#include "common.h"
#include "m2c.h"

typedef struct ALLowPass_s {
    s16 fc;
} ALLowPass;

typedef struct {
    u32 input;
    u32 output;
    s16 ffcoef;
    s16 fbcoef;
    s16 gain;
    u8 padE[0x10 - 0xE];
    f32 rsinc;
    f32 rsval;
    s32 rsdelta;
    f32 rsgain;
    ALLowPass *lp;
    u8 pad24[0x28 - 0x24];
} ALDelay;

typedef struct {
    u8 pad0[0x20];
    ALDelay *delay;
} ALFx;

typedef struct {
    u8 pad0[0x44];
    s32 outputRate;
} ALSynth;

extern ALSynth *alGlobals;
void init_lpfilter(ALLowPass *lp);

s32 alFxParamHdl(void *filter, s32 paramID, void *param) {
    ALFx *f = (ALFx *)filter;
    s32 p = (paramID - 2) % 8;
    s32 s = (paramID - 2) / 8;
    s32 val = *(s32 *)param;

    switch (p) {
    case 0:
        f->delay[s].input = (u32)val & 0xFFFFFFF8;
        break;
    case 1:
        f->delay[s].output = (u32)val & 0xFFFFFFF8;
        break;
    case 3:
        f->delay[s].ffcoef = (s16)val;
        break;
    case 2:
        f->delay[s].fbcoef = (s16)val;
        break;
    case 4:
        f->delay[s].gain = (s16)val;
        break;
    case 5:
        f->delay[s].rsinc = ((((f32)val) / 1000) * 2.0) / alGlobals->outputRate;
        break;
    case 6:
        f->delay[s].rsgain = (((f32)val) / 173123.404906676) * (f->delay[s].output - f->delay[s].input);
        break;
    case 7:
        if (f->delay[s].lp) {
            f->delay[s].lp->fc = (s16)val;
            init_lpfilter(f->delay[s].lp);
        }
        break;
    }
    return 0;
}
