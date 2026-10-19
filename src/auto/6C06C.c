#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w0;
    u32 w1;
} Acmd;
typedef struct ALFilter_s {
    struct ALFilter_s *source;
    Acmd *(*handler)(void *, s16 *, s32, s32, Acmd *);
    s32 (*setParam)(void *, s32, void *);
} ALFilter;
typedef struct ALPlayer_s {
    struct ALPlayer_s *next;
    void *clientData;
    s32 (*handler)(void *);
    s32 callTime;
    s32 samplesLeft;
} ALPlayer;
typedef struct {
    ALPlayer *head;
    u8 pad4[0x18];
    s32 paramSamples;
    s32 curSamples;
    u8 pad24[0x14];
    ALFilter *outputFilter;
    u8 pad3C[0xC];
    s32 maxOutSamples;
} ALSynth;
extern ALSynth *alGlobals;
s32 __nextSampleTime(ALSynth *, ALPlayer **);
s32 _timeToSamplesNoRound(ALSynth *, s32);
void _collectPVoices(ALSynth *);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

Acmd *alAudioFrame(Acmd *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen) {
    ALPlayer *client;
    ALFilter *output;
    ALSynth *drvr = alGlobals;
    s16 tmp = 0;
    Acmd *cmdlEnd = cmdList;
    Acmd *cmdPtr;
    s32 nOut;
    s16 *lOutBuf = outBuf;

    if (drvr->head == 0) {
        *cmdLen = 0;
        return cmdList;
    }
    for (drvr->paramSamples = __nextSampleTime(drvr, &client); drvr->paramSamples - drvr->curSamples < outLen;
         drvr->paramSamples = __nextSampleTime(drvr, &client)) {
        drvr->paramSamples &= ~0xF;
        client->samplesLeft += _timeToSamplesNoRound(drvr, (*client->handler)(client));
    }
    drvr->paramSamples &= ~0xF;
    while (outLen > 0) {
        nOut = MIN(drvr->maxOutSamples, outLen);
        cmdPtr = cmdlEnd;
        cmdPtr->w0 = 0x07000000;
        cmdPtr->w1 = 0;
        cmdPtr++;
        output = drvr->outputFilter;
        (*output->setParam)(output, 6, lOutBuf);
        cmdlEnd = (*output->handler)(output, &tmp, nOut, drvr->curSamples, cmdPtr);
        outLen -= nOut;
        lOutBuf += nOut << 1;
        drvr->curSamples += nOut;
    }
    *cmdLen = (s32)(cmdlEnd - cmdList);
    _collectPVoices(drvr);
    return cmdlEnd;
}
