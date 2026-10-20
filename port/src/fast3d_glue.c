/*
 * The system hooks the Fast3D sources from Project Midas call
 * (port/fast3d/system.h and a few profiling ones), on top of the port's own.
 */
#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdlib.h>

#include "system.h"

/* port.h is not included: system.h types s32 its own way */
void Port_Log(const char *fmt, ...);
void Port_Fatal(const char *fmt, ...);

s32 sysArgCheck(const char *arg) {
    return 0;
}

const char *sysArgGetString(const char *arg) {
    return NULL;
}

void sysFatalError(const char *fmt, ...) {
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    Port_Fatal("%s", buf);
    exit(1);
}

void sysLogPrintf(s32 level, const char *fmt, ...) {
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    Port_Log("%s", buf);
}

unsigned long long portProfNow(void) {
    return SDL_GetTicksNS() / 1000;
}

void portHitchNote(int cat, unsigned long long us) {
}

void portRoomCacheClear(void) {
}
