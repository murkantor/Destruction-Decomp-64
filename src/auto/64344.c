#include "common.h"
#include "m2c.h"

f32 Time_ToSeconds(u64 *t) {
    return (f32)*t * 1e-9;
}
