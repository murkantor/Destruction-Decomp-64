#include "common.h"
#include "m2c.h"

u32 osAiGetLength(void) {
    return *(vu32 *)0xA4500004;
}
