#include "common.h"
#include "m2c.h"

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;
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
struct PVoice_s;
typedef struct ALVoice_s {
    ALLink node;
    struct PVoice_s *pvoice;
    void *table;
    void *clientPrivate;
    s16 state;
    s16 priority;
    s16 fxBus;
    s16 unityPitch;
} ALVoice;
typedef struct PVoice_s {
    ALLink node;
    ALVoice *vvoice;
    ALFilter *channelKnob;
    u8 pad10[0xC8];
    s32 offset;
} PVoice;
typedef struct {
    s16 priority;
    s16 fxBus;
    u8 unityPitch;
} ALVoiceConfig;
typedef struct {
    u8 pad[0x1C];
    s32 paramSamples;
} ALSynth;
ALParam *__allocParam(void);
s32 _allocatePVoice(ALSynth *, PVoice **, s16);

s32 alSynAllocVoice(ALSynth *drvr, ALVoice *voice, ALVoiceConfig *vc) {
    PVoice *pvoice = 0;
    ALFilter *f;
    ALParam *update;
    s32 stolen;

    voice->priority = vc->priority;
    voice->unityPitch = vc->unityPitch;
    voice->table = 0;
    voice->fxBus = vc->fxBus;
    voice->state = 0;
    voice->pvoice = 0;
    stolen = _allocatePVoice(drvr, &pvoice, vc->priority);
    if (pvoice) {
        f = pvoice->channelKnob;
        if (stolen) {
            pvoice->offset = 512;
            pvoice->vvoice->pvoice = 0;
            update = __allocParam();
            update->delta = drvr->paramSamples;
            update->type = 11;
            update->data.i = 0;
            update->moredata.i = pvoice->offset - 64;
            (*f->setParam)(f, 3, update);
            update = __allocParam();
            if (update) {
                update->delta = drvr->paramSamples + pvoice->offset;
                update->type = 15;
                update->next = 0;
                (*f->setParam)(f, 3, update);
            }
        } else {
            pvoice->offset = 0;
        }
        pvoice->vvoice = voice;
        voice->pvoice = pvoice;
    }
    return (pvoice != 0);
}
