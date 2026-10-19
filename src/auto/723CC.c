#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x14];
    void *state;      /* 0x14 */
    s16 pan;          /* 0x18 */
    s16 volume;       /* 0x1A */
    s16 cvolL;        /* 0x1C */
    s16 cvolR;        /* 0x1E */
    s16 dryamt;       /* 0x20 */
    s16 wetamt;       /* 0x22 */
    u16 lratl;        /* 0x24 */
    s16 lratm;        /* 0x26 */
    s16 ltgt;         /* 0x28 */
    u16 rratl;        /* 0x2A */
    s16 rratm;        /* 0x2C */
    s16 rtgt;         /* 0x2E */
    s32 delta;        /* 0x30 */
    s32 segEnd;       /* 0x34 */
    s32 first;        /* 0x38 */
    void *ctrlList;   /* 0x3C */
    void *ctrlTail;   /* 0x40 */
    void *sources;    /* 0x44 */
    s32 motion;       /* 0x48 */
} ALEnvMixer;

extern void alFilterNew(void *f, void *pull, void *param, s32 type);
extern void *alHeapDBAlloc(u8 *file, s32 line, void *hp, s32 num, s32 size);
extern s32 alEnvmixerPull();
extern s32 alEnvmixerParam();

void alEnvmixerNew(ALEnvMixer *e, void *hp)
{
    alFilterNew(e, alEnvmixerPull, alEnvmixerParam, 4);
    e->state = alHeapDBAlloc(0, 0, hp, 1, 0x50);
    e->first = 1;
    e->motion = 0;
    e->volume = 1;
    e->ltgt = 1;
    e->rtgt = 1;
    e->cvolL = 1;
    e->cvolR = 1;
    e->dryamt = 0;
    e->wetamt = 0;
    e->lratm = 1;
    e->lratl = 0;
    e->lratm = 1;
    e->lratl = 0;
    e->delta = 0;
    e->segEnd = 0;
    e->pan = 0;
    e->ctrlList = 0;
    e->ctrlTail = 0;
    e->sources = 0;
}
