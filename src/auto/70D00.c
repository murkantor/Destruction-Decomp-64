#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ctrl; u32 width; u32 burst; u32 vSync; u32 hSync; u32 leap; u32 hStart; u32 xScale; u32 vCurrent;
} OSViCommonRegs;
typedef struct { u32 origin; u32 yScale; u32 vStart; u32 vBurst; u32 vIntr; } OSViFieldRegs;
typedef struct { u8 type; OSViCommonRegs comRegs; OSViFieldRegs fldRegs[2]; } OSViMode;
typedef struct { f32 factor; u16 offset; u32 scale; } __OSViScale;
typedef struct {
    u16 state; u16 retraceCount; void *framep; OSViMode *modep; u32 control;
    OSMesgQueue *msgq; OSMesg msg; __OSViScale x; __OSViScale y;
} __OSViContext;

extern __OSViContext *__osViCurr;
extern __OSViContext *__osViNext;
extern u32 __additional_scanline;
u32 osVirtualToPhysical(void *);

#define IO_READ(a) (*(volatile u32 *)(a))
#define IO_WRITE(a, v) (*(volatile u32 *)(a) = (v))

void __osViSwapContext(void) {
    register OSViMode *m;
    register __OSViContext *vc;
    u32 origin;
    u32 hStart;
    u32 vstart;
    u32 nomValue;
    u32 field;

    field = 0;
    vc = __osViNext;
    m = vc->modep;
    field = IO_READ(0xA4400010) & 1;
    origin = osVirtualToPhysical(vc->framep) + m->fldRegs[field].origin;
    if (vc->state & 2) {
        vc->x.scale |= m->comRegs.xScale & ~0xFFF;
    } else {
        vc->x.scale = m->comRegs.xScale;
    }
    if (vc->state & 4) {
        nomValue = m->fldRegs[field].yScale & 0xFFF;
        vc->y.scale = vc->y.factor * nomValue;
        vc->y.scale |= m->fldRegs[field].yScale & ~0xFFF;
    } else {
        vc->y.scale = m->fldRegs[field].yScale;
    }
    vstart = (m->fldRegs[field].vStart - (__additional_scanline << 16)) + __additional_scanline;
    hStart = m->comRegs.hStart;
    if (vc->state & 0x20) {
        hStart = 0;
    }
    if (vc->state & 0x40) {
        vc->y.scale = 0;
        origin = osVirtualToPhysical(vc->framep);
    }
    if (vc->state & 0x80) {
        vc->y.scale = (vc->y.offset << 16) & 0x3FF0000;
        origin = osVirtualToPhysical(vc->framep);
    }
    IO_WRITE(0xA4400004, origin);
    IO_WRITE(0xA4400008, m->comRegs.width);
    IO_WRITE(0xA4400014, m->comRegs.burst);
    IO_WRITE(0xA4400018, m->comRegs.vSync);
    IO_WRITE(0xA440001C, m->comRegs.hSync);
    IO_WRITE(0xA4400020, m->comRegs.leap);
    IO_WRITE(0xA4400024, hStart);
    IO_WRITE(0xA4400028, vstart);
    IO_WRITE(0xA440002C, m->fldRegs[field].vBurst);
    IO_WRITE(0xA440000C, m->fldRegs[field].vIntr);
    IO_WRITE(0xA4400030, vc->x.scale);
    IO_WRITE(0xA4400034, vc->y.scale);
    IO_WRITE(0xA4400000, vc->control);
    __osViNext = __osViCurr;
    __osViCurr = vc;
    *__osViNext = *__osViCurr;
}
