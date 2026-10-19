#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Acmd;
typedef struct {
    u8 pad0[0x14];
    void *state;
    f32 ratio;
    s32 upitch;
    f32 delta;
    s32 first;
} ALResampler;
typedef struct {
    u32 input;
    u32 output;
    u8 pad8[0x10];
    u32 rsdelta;
    u8 pad1C[0x8];
    ALResampler *rs;
} ALDelay;
typedef struct {
    u8 pad0[0x18];
    s16 *input;
} ALFx;

f32 _doModFunc(ALDelay *d, s32 count);
Acmd *_loadBuffer(ALFx *r, s16 *curr_ptr, s32 buff, s32 count, Acmd *p);
u32 osVirtualToPhysical(void *addr);

Acmd *_loadOutputBuffer(ALFx *r, ALDelay *d, s32 buff, s32 incount, Acmd *p)
{
    Acmd *ptr = p;
    s32 count;
    s16 *out_ptr;
    f32 fincount, fratio, delta;
    s32 ramalign = 0, length;

    if (d->rs) {
        length = d->output - d->input;
        delta = _doModFunc(d, incount);
        delta /= length;
        delta = (s32)(delta * 32768.0f);
        delta = delta / 32768.0f;
        fratio = 1.0 - delta;
        fincount = d->rs->delta + (fratio * (f32)incount);
        count = (s32)fincount;
        d->rs->delta = fincount - (f32)count;
        out_ptr = &r->input[-(d->output - d->rsdelta)];
        ramalign = ((s32)out_ptr & 0x7) >> 1;
        ptr = _loadBuffer(r, out_ptr - ramalign, 0x280, count + ramalign, ptr);
        {
            Acmd *_a = ptr++;
            _a->w0 = (0x08 << 24) | (((0x280 + (ramalign << 1))) & 0xffff);
            _a->w1 = ((buff & 0xffff) << 16) | ((incount << 1) & 0xffff);
        }
        {
            Acmd *_a = ptr++;
            _a->w0 = (0x05 << 24) | ((d->rs->first & 0xff) << 16) | ((s16)(fratio * 32768.0f) & 0xffff);
            _a->w1 = osVirtualToPhysical(d->rs->state);
        }
        d->rs->first = 0;
        d->rsdelta += count - incount;
    } else {
        out_ptr = &r->input[-d->output];
        ptr = _loadBuffer(r, out_ptr, buff, incount, ptr);
    }
    return ptr;
}
