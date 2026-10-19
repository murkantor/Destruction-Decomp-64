#include "common.h"
#include "m2c.h"

void osDpSetStatus(u32 data) {
    *(vu32 *)0xA410000C = data;
}
