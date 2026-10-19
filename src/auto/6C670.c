#include "common.h"
#include "m2c.h"

typedef struct ALParam_s {
    struct ALParam_s *next;
    s32 delta;
    s16 type;
    s16 unity;
    union {
        f32 f;
        s32 i;
        void *ptr;
    } data;
    union {
        f32 f;
        s32 i;
    } moredata;
} ALParam;
typedef struct ALFilter_s {
    struct ALFilter_s *source;
    s32 (*handler)(void *, s16 *, s32, s32, s32);
    s32 (*setParam)(void *, s32, void *);
} ALFilter;
typedef struct {
    u8 pad[0xC];
    ALFilter *channelKnob;
    u8 pad10[0xC8];
    s32 offset;
} PVoice;
typedef struct {
    u8 pad[8];
    PVoice *pvoice;
    u8 padC[0xE];
    u16 unityPitch;
} ALVoice;
typedef struct {
    u8 pad[0x1C];
    s32 paramSamples;
} ALSynth;
ALParam *__allocParam(void);
s32 _timeToSamples(ALSynth *, s32);

void alSynStopVoice(ALSynth *drvr, ALVoice *v) {
    ALParam *update;
    ALFilter *f;

    if (v->pvoice) {
        update = __allocParam();
        if (update == NULL) {
            return;
        }
        update->delta = drvr->paramSamples + v->pvoice->offset;
        update->type = 15;
        update->next = 0;
        f = v->pvoice->channelKnob;
        (*f->setParam)(f, 3, update);
    }
}
