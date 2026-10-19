#include "common.h"
#include "m2c.h"

extern void *__osRunningThread;

s32 osGetThreadPri(void *arg0) {
    void *var_a0;

    var_a0 = arg0;
    if (var_a0 == NULL) {
        var_a0 = __osRunningThread;
    }
    return M2C_FIELD(var_a0, s32 *, 4);
}
