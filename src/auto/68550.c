#include "common.h"
#include "m2c.h"

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;
typedef struct {
    ALLink node;
    u32 startAddr;
    u32 lastFrame;
    char *ptr;
} AMDMABuffer;
typedef struct {
    u8 pad0[0x10];
    void *dmaproc;
    void *heap;
    s32 outputRate;
} ALSynConfig;
typedef struct {
    u32 outputRate;
    u32 framesPerField;
    u32 maxACMDSize;
} amConfig;
typedef struct {
    s16 *data;
    u8 pad4[0x6C];
    s16 type;
    u8 pad72[2];
    void *task;
    u8 pad78[0x18];
} AudioInfo;
typedef struct {
    void *ACMDList[2];
    AudioInfo *audioInfo[3];
    OSThread thread;
    OSMesgQueue audioFrameMsgQ;
    OSMesg audioFrameMsgBuf[8];
    OSMesgQueue audioReplyMsgQ;
    OSMesg audioReplyMsgBuf[8];
    u8 g[1];
} AMAudioMgr;

extern void *D_800CEB1C;
extern void *D_800CEB20;
typedef struct { u8 initialized; } AMDMAState;
extern AMDMAState D_800CEAD0;
extern void *D_800CEB08;
extern s32 D_800CEB18;
extern AMDMABuffer *D_800CEB0C;
extern void *D_800CEB10;
extern OSMesg *D_800CEB14;
extern u32 D_800CEAE0;
extern s32 D_800CEADC;
extern s32 D_800CEAE4;
extern s32 D_800CEAE8;
extern AMAudioMgr D_8011F080;
extern OSMesgQueue D_800CEAF0;
extern s32 D_800B4E4C;
void *osCartRomInit(void);
s32 osAiSetFrequency(u32);
void *alHeapDBAlloc(u8 *, s32, void *, s32, s32);
void alLink(ALLink *, ALLink *);
void alInit(void *, ALSynConfig *);
void __amDmaNew(void);
void __amMain(void *);
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);

#define alHeapAlloc(hp, num, size) alHeapDBAlloc(0, 0, hp, num, size)

void amCreateAudioMgr(void *arg0, ALSynConfig *c, OSPri pri, amConfig *amc, s32 numBufs, s32 bufSize, s32 fps) {
    f32 fsize;
    u32 i;

    D_800CEB1C = osCartRomInit();
    D_800CEB20 = osCartRomInit();
    D_800CEAD0.initialized = 0;
    c->dmaproc = __amDmaNew;
    D_800CEB08 = arg0;
    D_800CEB18 = bufSize;
    c->outputRate = osAiSetFrequency(amc->outputRate);
    D_800CEB0C = alHeapAlloc(c->heap, 1, numBufs * 20);
    D_800CEB10 = alHeapAlloc(c->heap, 1, numBufs * 48);
    D_800CEB14 = alHeapAlloc(c->heap, 1, numBufs * 8);
    fsize = (f32)amc->framesPerField * c->outputRate / (f32)fps;
    D_800CEAE0 = (s32)fsize;
    if (D_800CEAE0 < fsize) {
        D_800CEAE0++;
    }
    if (D_800CEAE0 & 0xF) {
        D_800CEAE0 = (D_800CEAE0 & ~0xF) + 0x10;
    }
    D_800CEADC = D_800CEAE0 - 16;
    D_800CEAE4 = D_800CEAE0 + 0x60;
    alInit(D_8011F080.g, c);
    D_800CEB0C[0].node.prev = NULL;
    D_800CEB0C[0].node.next = NULL;
    for (i = 0; i < numBufs - 1; i++) {
        alLink(&D_800CEB0C[i + 1].node, &D_800CEB0C[i].node);
        D_800CEB0C[i].ptr = alHeapAlloc(c->heap, 1, bufSize);
    }
    D_800CEB0C[i].ptr = alHeapAlloc(c->heap, 1, bufSize);
    for (i = 0; i < 2; i++) {
        D_8011F080.ACMDList[i] = alHeapAlloc(c->heap, 1, amc->maxACMDSize * 8);
    }
    D_800CEAE8 = amc->maxACMDSize;
    for (i = 0; i < 3; i++) {
        D_8011F080.audioInfo[i] = alHeapAlloc(c->heap, 1, sizeof(AudioInfo));
        D_8011F080.audioInfo[i]->type = 2;
        D_8011F080.audioInfo[i]->task = D_8011F080.audioInfo[i];
        D_8011F080.audioInfo[i]->data = alHeapAlloc(c->heap, 1, 4 * D_800CEAE4);
    }
    osCreateMesgQueue(&D_8011F080.audioReplyMsgQ, D_8011F080.audioReplyMsgBuf, 8);
    osCreateMesgQueue(&D_8011F080.audioFrameMsgQ, D_8011F080.audioFrameMsgBuf, 8);
    osCreateMesgQueue(&D_800CEAF0, D_800CEB14, numBufs * 2);
    if (D_800B4E4C == 0) {
        osCreateThread(&D_8011F080.thread, 3, __amMain, NULL, &D_800CEAD0, pri);
    }
    osStartThread(&D_8011F080.thread);
    D_800B4E4C = 1;
}
