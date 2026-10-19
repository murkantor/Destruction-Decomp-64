#include "common.h"
#include "m2c.h"

s32 alAuxBusPull(void *, s32 *, s32, s32, s32);
s32 alAuxBusParam(void *, s32, s32);
void alFilterNew(void *, void *, void *, s32);
typedef struct {
    u8 filter[0x14];
    s32 sourceCount;
    s32 maxSources;
    void **sources;
} ALAuxBus;

void alAuxBusNew(ALAuxBus *m, void *sources, s32 maxSources) {
    alFilterNew(m, alAuxBusPull, alAuxBusParam, 6);
    m->sourceCount = 0;
    m->maxSources = maxSources;
    m->sources = sources;
}
