#include "common.h"
#include "m2c.h"

void Racer_UpdateLapTiming();
void func_8001FA48();

void func_8001F6C4(s32 arg0, u8 flag) {
    if (flag) {
        Racer_UpdateLapTiming(arg0);
    } else {
        func_8001FA48(arg0);
    }
}
