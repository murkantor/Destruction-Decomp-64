#include "common.h"
#include "m2c.h"

extern s32 __osPiDevMgr;
extern void *D_800B5728;

void *osPiGetCmdQueue(void) {
    if (!__osPiDevMgr) {
        return NULL;
    }
    return D_800B5728;
}
