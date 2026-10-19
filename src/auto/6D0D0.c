#include "common.h"
#include "m2c.h"

typedef struct {
    u16 state;
} __OSViContext;
extern __OSViContext *__osViNext;
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void osViBlack(u8 active) {
    register u32 saveMask = __osDisableInt();

    if (active) {
        __osViNext->state |= 0x20;
    } else {
        __osViNext->state &= ~0x20;
    }
    __osRestoreInt(saveMask);
}
