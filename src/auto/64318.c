#include "common.h"
#include "m2c.h"

f32 Clock_GetDeltaSeconds(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0x10);
}
