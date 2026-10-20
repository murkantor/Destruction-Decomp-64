/*
 * Entry point of the PC port. Loads the ROM and the RAM image, opens the window,
 * starts the game's own boot() (which creates the N64 threads), then runs the
 * host side on this thread: SDL events, the RCP tasks and the 60 Hz VI clock.
 */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "port.h"
#include "port_config.h"

#define VI_RATE_NS (1000000000ull / 60)

void boot(void);

volatile int gPortQuit;
static FILE *sLogFile;

void Port_Log(const char *fmt, ...) {
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fprintf(stderr, "%s\n", buf);
    if (sLogFile != NULL) {
        fprintf(sLogFile, "%s\n", buf);
        fflush(sLogFile);
    }
}

void Port_Fatal(const char *fmt, ...) {
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    Port_Log("fatal: %s", buf);
    if (SDL_getenv("DD64_EXIT_AFTER") == NULL) { /* scripted test runs: no dialog to wait on */
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Destruction Derby 64", buf, NULL);
    }
    exit(1);
}

void Port_RequestQuit(void) {
    gPortQuit = 1;
}

static void openLog(void) {
    const char *base = SDL_GetBasePath();
    char path[1024];

    SDL_snprintf(path, sizeof(path), "%sData/Logs", base != NULL ? base : "");
    SDL_CreateDirectory(path);
    SDL_snprintf(path, sizeof(path), "%sData/Logs/dd64.log", base != NULL ? base : "");
    sLogFile = fopen(path, "w");
}

int main(int argc, char **argv) {
    u64 nextRetrace;

    openLog();
    Port_ConfigLoad();
    SDL_SetAppMetadata("Destruction Derby 64", "0.1", "dd64.pc");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO)) {
        Port_Fatal("SDL_Init: %s", SDL_GetError());
    }
    if (!Port_RomLoad()) {
        Port_Fatal("Could not find the Destruction Derby 64 (USA) ROM.\n\n"
                   "Put your cart image (.z64, .n64 or .v64) in the Data folder next to the game.");
    }
    Port_RamInit();
    Port_OsInit();
    Port_RcpInit();
    Port_VideoInit();
    Port_AudioInit();
    Port_InputInit();

    boot();

    nextRetrace = SDL_GetTicksNS() + VI_RATE_NS;
    while (!gPortQuit) {
        u64 now;

        Port_PumpEvents();
        Port_RcpRunPending();
        Port_OsHostTick();
        now = SDL_GetTicksNS();
        if (now >= nextRetrace) {
            Port_ViRetrace();
            nextRetrace += VI_RATE_NS;
            if (now > nextRetrace + 4 * VI_RATE_NS) {
                nextRetrace = now + VI_RATE_NS; /* fell behind; don't try to catch up */
            }
        }
        Port_RcpWait(SDL_min(nextRetrace, now + 1000000));
    }

    SDL_Quit();
    return 0;
}
