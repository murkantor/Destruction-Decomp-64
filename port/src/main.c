/*
 * Entry point of the PC port. Loads the ROM and the RAM image, opens the window,
 * starts the game's own boot() (which creates the N64 threads), then runs the
 * host side on this thread: SDL events, the RCP tasks and the 60 Hz VI clock.
 */
#ifndef _WIN32
#define _GNU_SOURCE /* the register names in ucontext.h (the crash handler) */
#endif
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
#else
#include <execinfo.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

/* A crash on Linux: log the signal, the faulting address, the registers and the
   call chain (the executable is not position-independent: addr2line -e
   DestructionDerby64 takes the addresses as they are), then die as before */
static void crashHandler(int sig, siginfo_t *si, void *uc) {
    const mcontext_t *m = &((const ucontext_t *)uc)->uc_mcontext;
    void *frames[32];
    int n;

    Port_Log("crash: signal %d at %08X, accessing %p", sig, (unsigned)m->gregs[REG_EIP], si->si_addr);
    Port_Log("crash: eax %08X ebx %08X ecx %08X edx %08X esi %08X edi %08X ebp %08X esp %08X",
             (unsigned)m->gregs[REG_EAX], (unsigned)m->gregs[REG_EBX], (unsigned)m->gregs[REG_ECX],
             (unsigned)m->gregs[REG_EDX], (unsigned)m->gregs[REG_ESI], (unsigned)m->gregs[REG_EDI],
             (unsigned)m->gregs[REG_EBP], (unsigned)m->gregs[REG_ESP]);
    {
        /* the top of the stack: after a call through a null pointer (eip 0) the
           caller's return address is the first word, which backtrace misses */
        const u32 *sp = (const u32 *)m->gregs[REG_ESP];
        int i;
        for (i = 0; i < 16; i += 4) {
            Port_Log("crash: stack +%02X %08X %08X %08X %08X", i * 4, sp[i], sp[i + 1], sp[i + 2], sp[i + 3]);
        }
    }
    n = backtrace(frames, 32);
    if (sLogFile != NULL) {
        fflush(sLogFile);
        backtrace_symbols_fd(frames, n, fileno(sLogFile));
    }
    backtrace_symbols_fd(frames, n, STDERR_FILENO);
    signal(sig, SIG_DFL);
    raise(sig);
}

static void installCrashHandler(void) {
    static const int sigs[] = { SIGSEGV, SIGBUS, SIGILL, SIGFPE, SIGABRT };
    struct sigaction sa;
    size_t i;

    SDL_zero(sa);
    sa.sa_sigaction = crashHandler;
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    for (i = 0; i < SDL_arraysize(sigs); i++) {
        sigaction(sigs[i], &sa, NULL);
    }
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
#else
    installCrashHandler();
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

    {
        void Port_ProfileStart(void);
        Port_ProfileStart(); /* DD64_PROFILE (testing) */
    }
    boot();
    Port_ViStart();

    while (!gPortQuit) {
        Port_PumpEvents();
        Port_RcpRunPending();
        Port_ViRetrace();
        Port_VideoPresentInterp();
        Port_OptionsApplyPending();
        Port_RcpWait(SDL_GetTicksNS() + 1000000); /* a task, a retrace, or a millisecond */
    }

    Port_RaphnetStop();
    SDL_Quit();
    return 0;
}
