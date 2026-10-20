/*
 * The RSP and RDP: tasks the scheduler starts with osSpTaskStartGo are queued
 * here and run in order on the host main thread, which owns the GL context.
 * When a task is done the SP interrupt is raised, and for a graphics task the
 * DP interrupt after it, as the SDK scheduler expects.
 */
#include <SDL3/SDL.h>

#include "port.h"

#define MAX_TASKS 16

static SDL_Mutex *sLock;
static SDL_Condition *sCond;
static OSTask *sQueue[MAX_TASKS];
static int sHead, sCount;

void Port_RcpInit(void) {
    sLock = SDL_CreateMutex();
    sCond = SDL_CreateCondition();
}

void Port_RcpSubmit(OSTask *task) {
    SDL_LockMutex(sLock);
    if (sCount == MAX_TASKS) {
        Port_Fatal("RCP task queue full");
    }
    sQueue[(sHead + sCount) % MAX_TASKS] = task;
    sCount++;
    SDL_SignalCondition(sCond);
    SDL_UnlockMutex(sLock);
}

/* Sleep until a task arrives or until untilNs. */
void Port_RcpWait(u64 untilNs) {
    u64 now = SDL_GetTicksNS();

    if (untilNs <= now) {
        return;
    }
    SDL_LockMutex(sLock);
    if (sCount == 0) {
        Sint32 ms = (Sint32)((untilNs - now + 999999) / 1000000);
        SDL_WaitConditionTimeout(sCond, sLock, ms);
    }
    SDL_UnlockMutex(sLock);
}

void Port_RcpRunPending(void) {
    for (;;) {
        OSTask *task;

        SDL_LockMutex(sLock);
        if (sCount == 0) {
            SDL_UnlockMutex(sLock);
            return;
        }
        task = sQueue[sHead];
        sHead = (sHead + 1) % MAX_TASKS;
        sCount--;
        SDL_UnlockMutex(sLock);

        switch (task->t.type) {
            case M_GFXTASK:
                Port_VideoRunTask(task);
                Port_PostEvent(OS_EVENT_SP);
                Port_PostEvent(OS_EVENT_DP);
                break;
            case M_AUDTASK:
                Port_AudioRunTask(task);
                Port_PostEvent(OS_EVENT_SP);
                break;
            default:
                Port_Log("RCP: unknown task type %u", (unsigned)task->t.type);
                Port_PostEvent(OS_EVENT_SP);
                break;
        }
    }
}
