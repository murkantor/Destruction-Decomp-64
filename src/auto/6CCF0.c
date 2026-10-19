#include "common.h"
#include "m2c.h"

s32 __osSpSetPc(u32 pc) {
    register u32 status = *(vu32 *)0xA4040010;

    if (!(status & 1)) {
        return -1;
    } else {
        *(vu32 *)0xA4080000 = pc;
        return 0;
    }
}

typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
} __OSViContext;
extern __OSViContext *__osViCurr;
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void *osViGetCurrentFramebuffer(void) {
    register u32 saveMask = __osDisableInt();
    void *framep = __osViCurr->framep;

    __osRestoreInt(saveMask);
    return framep;
}
