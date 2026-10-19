#include "common.h"
#include "m2c.h"

double frexp(double value, int *eptr) {
    if (value == 0.0) {
        *eptr = 0;
        return 0.0;
    }
    *eptr = ((*(u32 *)&value >> 20) & 0x7FF) - 0x3FE;
    *(u32 *)&value = (*(u32 *)&value & 0x800FFFFF) | 0x3FE00000;
    return value;
}
