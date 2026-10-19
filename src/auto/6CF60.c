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

void osViSetXScale(f32 value) {
    register u32 nomValue;
    register u32 saveMask = __osDisableInt();

    __osViNext->x.factor = value;
    __osViNext->state |= 2;
    nomValue = __osViNext->modep->comRegs.xScale & 0xFFF;
    __osViNext->x.scale = (u32)(__osViNext->x.factor * nomValue) & 0xFFF;
    __osRestoreInt(saveMask);
}
