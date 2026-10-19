#include "common.h"
#include "m2c.h"

u32 osDpGetStatus(void) {
    return *(vu32 *)0xA410000C;
}
