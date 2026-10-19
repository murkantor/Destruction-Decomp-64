#include "common.h"
#include "m2c.h"

typedef struct { void *source; void *handler; void *setParam; s16 inp; s16 outp; s32 type; } ALFilter;
typedef struct {
    ALFilter filter;
    void *state;
    f32 ratio;
    s32 upitch;
    f32 delta;
    s32 first;
    void *ctrlList;
    void *ctrlTail;
    s32 motion;
} ALResampler;
extern void alFilterNew(ALFilter *f, void *h, void *s, s32 type);
extern void *alHeapDBAlloc(u8 *file, s32 line, void *hp, s32 num, s32 size);
extern s32 alResamplePull();
extern s32 alResampleParam();

void alResampleNew(ALResampler *r, void *hp) {
    alFilterNew((ALFilter *)r, alResamplePull, alResampleParam, 1);
    r->state = alHeapDBAlloc(0, 0, hp, 1, 0x20);
    r->delta = 0.0;
    r->first = 1;
    r->motion = 0;
    r->ratio = 1.0;
    r->upitch = 0;
    r->ctrlList = 0;
    r->ctrlTail = 0;
}
