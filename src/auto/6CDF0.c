#include "common.h"
#include "m2c.h"

typedef struct {
    u8 type;
    struct {
        u32 ctrl;
        u32 width;
        u32 burst;
        u32 vSync;
        u32 hSync;
        u32 leap;
        u32 hStart;
        u32 xScale;
        u32 vCurrent;
    } comRegs;
} OSViMode;
typedef struct {
    f32 factor;
    u16 offset;
    u32 scale;
} __OSViScale;
typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
    OSViMode *modep;
    u32 control;
    OSMesgQueue *msgq;
    OSMesg msg;
    __OSViScale x;
    __OSViScale y;
} __OSViContext;
extern __OSViContext *__osViNext;
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void osViSetSpecialFeatures(u32 func) {
    register u32 saveMask = __osDisableInt();

    if (func & 1) {
        __osViNext->control |= 8;
    }
    if (func & 2) {
        __osViNext->control &= ~8;
    }
    if (func & 4) {
        __osViNext->control |= 4;
    }
    if (func & 8) {
        __osViNext->control &= ~4;
    }
    if (func & 0x10) {
        __osViNext->control |= 0x10;
    }
    if (func & 0x20) {
        __osViNext->control &= ~0x10;
    }
    if (func & 0x40) {
        __osViNext->control |= 0x10000;
        __osViNext->control &= ~0x300;
    }
    if (func & 0x80) {
        __osViNext->control &= ~0x10000;
        __osViNext->control |= __osViNext->modep->comRegs.ctrl & 0x300;
    }
    __osViNext->state |= 8;
    __osRestoreInt(saveMask);
}
