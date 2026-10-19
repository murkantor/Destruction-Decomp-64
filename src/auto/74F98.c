#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    f32 rsinc;
    f32 rsval;
    s32 rsdelta;
    f32 rsgain;
} ALDelay;

f32 _doModFunc(ALDelay *d, s32 count)
{
    f32 val;

    d->rsval += d->rsinc * count;
    d->rsval = (d->rsval > 2.0) ? d->rsval - 4.0 : d->rsval;
    val = d->rsval;
    val = (val < 0) ? -val : val;
    val = val - 1.0;
    return d->rsgain * val;
}
