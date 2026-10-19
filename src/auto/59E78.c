#include "common.h"
#include "m2c.h"

extern s32 gRumbleRetry;

void Rumble_AllowRetry(void) {
    gRumbleRetry = 1;
}


void Rumble_DisallowRetry(void) {
    gRumbleRetry = 0;
}
