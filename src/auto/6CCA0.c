#include "common.h"
#include "m2c.h"

void osDpGetCounters(u32 *array) {
    *array++ = *(vu32 *)0xA4100010;
    *array++ = *(vu32 *)0xA4100014;
    *array++ = *(vu32 *)0xA4100018;
    *array++ = *(vu32 *)0xA410001C;
}
