#include "common.h"
#include "m2c.h"

f64 log();                                /* extern */
extern f64 D_800BC808;

f64 log10(void) {
    return log() * D_800BC808;
}
