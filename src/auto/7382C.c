#include "common.h"
#include "m2c.h"

typedef struct ALFilter_s {
    struct ALFilter_s *source;
    s32 (*handler)();
    s32 (*setParam)(void *, s32, void *);
} ALFilter;

typedef struct {
    ALFilter *source;
    u8 pad4[0x1A - 4];
    s16 volume;
    u8 pad1C[0x38 - 0x1C];
    s32 first;
    void *ctrlList;
    void **ctrlTail;
    u8 pad44[4];
    s32 motion;
} ALEnvMixer;

s32 alEnvmixerParam(void *filter, s32 paramID, void *param)
{
    ALFilter *f = (ALFilter *)filter;
    ALEnvMixer *e = (ALEnvMixer *)filter;

    switch (paramID) {
        case 3:
            if (e->ctrlTail) {
                *e->ctrlTail = param;
            } else {
                e->ctrlList = param;
            }
            e->ctrlTail = param;
            break;
        case 4:
            e->first = 1;
            e->motion = 0;
            e->volume = 1;
            if (f->source)
                (*f->source->setParam)(f->source, 4, param);
            break;
        case 9:
            e->motion = 1;
            if (f->source)
                (*f->source->setParam)(f->source, 9, param);
            break;
        case 1:
            f->source = (ALFilter *)param;
            break;
        default:
            if (f->source)
                (*f->source->setParam)(f->source, paramID, param);
            break;
    }
    return 0;
}
