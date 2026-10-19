#include "common.h"
#include "m2c.h"

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;
typedef struct ALParam_s {
    struct ALParam_s *next;
    u8 pad4[0x18];
} ALParam;
typedef struct {
    ALLink node;
    void *vvoice;
    void *channelKnob;
    u8 decoder[0x48];
    u8 resampler[0x34];
    u8 envmixer[0x50];
} PVoice;
typedef struct {
    u8 pad[4];
    s32 maxPVoices;
    s32 maxUpdates;
    u8 padC[4];
    void *dmaproc;
    void *heap;
    s32 outputRate;
    u8 fxType;
} ALSynConfig;
typedef struct {
    void *head;
    ALLink pFreeList;
    ALLink pAllocList;
    ALLink pLameList;
    s32 paramSamples;
    s32 curSamples;
    void *dma;
    void *heap;
    ALParam *paramList;
    void *mainBus;
    void *auxBus;
    void *outputFilter;
    s32 numPVoices;
    s32 maxAuxBusses;
    s32 outputRate;
    s32 maxOutSamples;
} ALSynth;
void *alHeapDBAlloc(u8 *, s32, void *, s32, s32);
void alSaveNew(void *);
void alAuxBusNew(void *, void *, s32);
void alMainBusNew(void *, void *, s32);
void *alSynAllocFX(ALSynth *, s16, ALSynConfig *, void *);
s32 alMainBusParam(void *, s32, void *);
s32 alSaveParam(void *, s32, void *);
void alLink(ALLink *, ALLink *);
void alLoadNew(void *, void *, void *);
s32 alLoadParam(void *, s32, void *);
void alResampleNew(void *, void *);
s32 alResampleParam(void *, s32, void *);
void alEnvmixerNew(void *, void *);
s32 alEnvmixerParam(void *, s32, void *);
s32 alAuxBusParam(void *, s32, void *);

#define alHeapAlloc(hp, num, size) alHeapDBAlloc(0, 0, hp, num, size)

void alSynNew(ALSynth *drvr, ALSynConfig *c) {
    s32 i;
    PVoice *pv;
    PVoice *pvoices;
    ALParam *paramPtr;
    void *hp = c->heap;
    void *save;
    void *sources;
    ALParam *params;

    drvr->head = NULL;
    drvr->numPVoices = c->maxPVoices;
    drvr->curSamples = 0;
    drvr->paramSamples = 0;
    drvr->outputRate = c->outputRate;
    drvr->maxOutSamples = 160;
    drvr->dma = c->dmaproc;
    save = alHeapAlloc(hp, 1, 0x1C);
    alSaveNew(save);
    drvr->outputFilter = save;
    drvr->auxBus = alHeapAlloc(hp, 1, 0x4C);
    drvr->maxAuxBusses = 1;
    sources = alHeapAlloc(hp, c->maxPVoices, 4);
    alAuxBusNew(drvr->auxBus, sources, c->maxPVoices);
    drvr->mainBus = alHeapAlloc(hp, 1, 0x20);
    sources = alHeapAlloc(hp, c->maxPVoices, 4);
    alMainBusNew(drvr->mainBus, sources, c->maxPVoices);
    if (c->fxType != 0) {
        alSynAllocFX(drvr, 0, c, hp);
    } else {
        alMainBusParam(drvr->mainBus, 2, drvr->auxBus);
    }
    drvr->pFreeList.next = 0;
    drvr->pFreeList.prev = 0;
    drvr->pLameList.next = 0;
    drvr->pLameList.prev = 0;
    drvr->pAllocList.next = 0;
    drvr->pAllocList.prev = 0;
    pvoices = alHeapAlloc(hp, c->maxPVoices, sizeof(PVoice));
    for (i = 0; i < c->maxPVoices; i++) {
        pv = &pvoices[i];
        alLink(&pv->node, &drvr->pFreeList);
        pv->vvoice = 0;
        alLoadNew(pv->decoder, drvr->dma, hp);
        alLoadParam(pv->decoder, 1, 0);
        alResampleNew(pv->resampler, hp);
        alResampleParam(pv->resampler, 1, pv->decoder);
        alEnvmixerNew(pv->envmixer, hp);
        alEnvmixerParam(pv->envmixer, 1, pv->resampler);
        alAuxBusParam(drvr->auxBus, 2, pv->envmixer);
        pv->channelKnob = pv->envmixer;
    }
    alSaveParam(save, 1, drvr->mainBus);
    params = alHeapAlloc(hp, c->maxUpdates, sizeof(ALParam));
    drvr->paramList = 0;
    for (i = 0; i < c->maxUpdates; i++) {
        paramPtr = &params[i];
        paramPtr->next = drvr->paramList;
        drvr->paramList = paramPtr;
    }
    drvr->heap = hp;
}
