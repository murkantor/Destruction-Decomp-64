#include "common.h"
#include "m2c.h"

typedef struct { s32 start; s32 end; s32 count; s16 state[16]; } ALADPCMloop;
typedef struct { s32 order; s32 npredictors; } ALADPCMBook;
typedef struct { s32 start; s32 end; s32 count; } ALRawLoop;
typedef struct {
    u8 *base;
    s32 len;
    u8 type;
    u8 flags;
    union {
        struct { ALADPCMloop *loop; ALADPCMBook *book; } adpcmWave;
        struct { ALRawLoop *loop; } rawWave;
    } waveInfo;
} ALWaveTable;
typedef struct {
    void *source;
    void *handler;
    void *setParam;
    s16 inp;
    s16 outp;
    s32 type;
    void *state;
    void *lstate;
    ALRawLoop loop;
    ALWaveTable *table;
    s32 bookSize;
    void *dma;
    void *dmaState;
    s32 sample;
    s32 lastsam;
    s32 first;
    s32 memin;
} ALLoadFilter;

extern s32 alAdpcmPull();
extern s32 func_80071F04();
void alCopy(void *src, void *dest, s32 len);

void alLoadParam(void *filter, s32 paramID, void *param)
{
    ALLoadFilter *a = (ALLoadFilter *)filter;

    switch (paramID) {
        case 5:
            a->table = (ALWaveTable *)param;
            a->memin = (s32)a->table->base;
            a->sample = 0;
            switch (a->table->type) {
                case 0:
                    a->handler = alAdpcmPull;
                    a->table->len = 9 * ((s32)(a->table->len / 9));
                    a->bookSize = 2 * a->table->waveInfo.adpcmWave.book->order *
                        a->table->waveInfo.adpcmWave.book->npredictors * 8;
                    if (a->table->waveInfo.adpcmWave.loop) {
                        a->loop.start = a->table->waveInfo.adpcmWave.loop->start;
                        a->loop.end = a->table->waveInfo.adpcmWave.loop->end;
                        a->loop.count = a->table->waveInfo.adpcmWave.loop->count;
                        alCopy(a->table->waveInfo.adpcmWave.loop->state, a->lstate, 32);
                    } else {
                        a->loop.start = a->loop.end = a->loop.count = 0;
                    }
                    break;
                case 1:
                    a->handler = func_80071F04;
                    if (a->table->waveInfo.rawWave.loop) {
                        a->loop.start = a->table->waveInfo.rawWave.loop->start;
                        a->loop.end = a->table->waveInfo.rawWave.loop->end;
                        a->loop.count = a->table->waveInfo.rawWave.loop->count;
                    } else {
                        a->loop.start = a->loop.end = a->loop.count = 0;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 4:
            a->lastsam = 0;
            a->first = 1;
            a->sample = 0;
            if (a->table) {
                a->memin = (s32)a->table->base;
                if (a->table->type == 0) {
                    if (a->table->waveInfo.adpcmWave.loop)
                        a->loop.count = a->table->waveInfo.adpcmWave.loop->count;
                } else if (a->table->type == 1) {
                    if (a->table->waveInfo.rawWave.loop)
                        a->loop.count = a->table->waveInfo.rawWave.loop->count;
                }
            }
            break;
        default:
            break;
    }
}
