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
#include "port_raphnet.h"

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

#ifdef _WIN32
#include <windows.h>

/* A crash on Windows: log the faulting address and the frame-pointer chain
   (all code is built with frame pointers), for i686-w64-mingw32-addr2line */
static LONG WINAPI crashHandler(EXCEPTION_POINTERS *info) {
    CONTEXT *c = info->ContextRecord;
    u32 *fp = (u32 *)c->Ebp;
    int i;

    Port_Log("crash: exception %08lX at %p, accessing %p", (unsigned long)info->ExceptionRecord->ExceptionCode,
             info->ExceptionRecord->ExceptionAddress,
             info->ExceptionRecord->NumberParameters >= 2 ? (void *)info->ExceptionRecord->ExceptionInformation[1]
                                                          : NULL);
    Port_Log("crash: eax %08lX ebx %08lX ecx %08lX edx %08lX esi %08lX edi %08lX ebp %08lX esp %08lX",
             c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
    for (i = 0; i < 24 && fp != NULL && !IsBadReadPtr(fp, 8); i++) {
        Port_Log("crash: frame %d return %08X", i, fp[1]);
        if ((u32 *)fp[0] <= fp) {
            break;
        }
        fp = (u32 *)fp[0];
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

static void openLog(void) {
    const char *base = SDL_GetBasePath();
    char path[1024];

    SDL_snprintf(path, sizeof(path), "%sData/Logs", base != NULL ? base : "");
    SDL_CreateDirectory(path);
    SDL_snprintf(path, sizeof(path), "%sData/Logs/dd64.log", base != NULL ? base : "");
    sLogFile = fopen(path, "w");
}

int main(int argc, char **argv) {
    openLog();
#ifdef _WIN32
    SetUnhandledExceptionFilter(crashHandler);
#endif
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
    Port_ViStart();

    while (!gPortQuit) {
        Port_PumpEvents();
        Port_RcpRunPending();
        Port_OsHostTick();
        Port_ViRetrace();
        Port_RcpWait(SDL_GetTicksNS() + 1000000); /* a task, a retrace, or a millisecond */
    }

    Port_RaphnetStop();
    SDL_Quit();
    return 0;
}
