#include "common.h"
#include "m2c.h"

u32 osPiGetStatus(void) {
    return *(vu32 *)0xA4600010;
}
