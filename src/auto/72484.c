#include "common.h"
#include "m2c.h"

typedef s32 (*ALDMAproc)(s32, s32, void *);
typedef ALDMAproc (*ALDMANew)(void *);
typedef struct {
    u8 pad[0x14];
    void *state;
    void *lstate;
    u8 pad1C[0x30 - 0x1C];
    ALDMAproc dma;
    void *dmaState;
    u8 pad38[4];
    s32 lastsam;
    s32 first;
    s32 memin;
} ALLoadFilter;

extern void alFilterNew(void *f, void *pull, void *param, s32 type);
extern void *alHeapDBAlloc(u8 *file, s32 line, void *hp, s32 num, s32 size);
extern s32 func_80071AC0();
extern s32 alLoadParam();

void alLoadNew(ALLoadFilter *f, ALDMANew dmaNew, void *hp)
{
    alFilterNew(f, func_80071AC0, alLoadParam, 0);
    f->state = alHeapDBAlloc(0, 0, hp, 1, 32);
    f->lstate = alHeapDBAlloc(0, 0, hp, 1, 32);
    f->dma = dmaNew(&f->dmaState);
    f->lastsam = 0;
    f->first = 1;
    f->memin = 0;
}
