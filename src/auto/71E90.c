#include "common.h"
#include "m2c.h"

#define SCALE 16384
typedef struct { s16 fc; s16 fgain; union { s16 fccoef[16]; s64 force_aligned; } fcvec; void *fstate; s32 first; } ALLowPass;

void init_lpfilter(ALLowPass *lp) {
    s32 i, temp;
    s16 fc;
    f64 ffc, fcoef;

    temp = lp->fc * SCALE;
    fc = temp >> 15;
    lp->fgain = SCALE - fc;

    lp->first = 1;
    for (i = 0; i < 8; i++)
        lp->fcvec.fccoef[i] = 0;

    lp->fcvec.fccoef[i++] = fc;
    fcoef = ffc = (f64)fc / SCALE;

    for (; i < 16; i++) {
        fcoef *= ffc;
        lp->fcvec.fccoef[i] = (s16)(fcoef * SCALE);
    }
}
