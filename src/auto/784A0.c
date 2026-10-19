#include "common.h"
#include "m2c.h"

u32 __osSpGetStatus(void) {
    return *(vu32 *)0xA4040010;
}

void __osSpSetStatus(u32 data) {
    *(vu32 *)0xA4040010 = data;
}
