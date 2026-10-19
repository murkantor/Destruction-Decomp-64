#include "common.h"
#include "m2c.h"

typedef struct {
    u16 state;
    u16 retraceCount;
    void *framep;
    u32 *modep;
    u32 control;
} __OSViContext;
extern __OSViContext *__osViNext;
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void osViSetMode(u32 *modep) {
    register u32 saveMask = __osDisableInt();

    __osViNext->modep = modep;
    __osViNext->state = 1;
    __osViNext->control = __osViNext->modep[1];
    __osRestoreInt(saveMask);
}
