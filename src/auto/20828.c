#include "common.h"
#include "m2c.h"

f64 atan2(f64, f64);                        /* extern */

f32 func_8001FC28(void *arg0) {
    return (f32) atan2((f64) M2C_FIELD(arg0, f32 *, 0x444), (f64) M2C_FIELD(arg0, f32 *, 0x44C));
}
