#include "common.h"
#include "m2c.h"

typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
} __OSViContext;
extern __OSViContext *__osViNext;
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void *osViGetNextFramebuffer(void) {
    register u32 saveMask = __osDisableInt();
    void *framep = __osViNext->framep;

    __osRestoreInt(saveMask);
    return framep;
}
