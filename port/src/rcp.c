/*
 * The RSP and RDP. Graphics tasks the scheduler starts with osSpTaskStartGo
 * are queued here and run in order on the host main thread, which owns the GL
 * context; when one is done the SP interrupt is raised, then the DP interrupt,
 * as the SDK scheduler expects. Audio tasks need no GL context and must not
 * wait for the graphics (the main thread can be busy for a long time loading
 * textures): they run at once on the thread that starts them.
 *
 * The scheduler asks the graphics task to yield when an audio task is
 * waiting (osSpTaskYield). A task still queued is taken off the queue; one
 * being drawn goes on being drawn, but counts as yielded, and the scheduler
 * starts it again once the audio task is done, which here means waiting for
 * that drawing to finish (the RSP would resume from its saved state).
 */
#include <SDL3/SDL.h>

#include "port.h"

#define MAX_TASKS 16

static SDL_Mutex *sLock;
static SDL_Condition *sCond;
static OSTask *sQueue[MAX_TASKS];
static u64 sQueuedNs[MAX_TASKS]; /* when each was queued (DD64_GFXLOG timings) */
static int sHead, sCount;
static OSTask *sDrawing;     /* the graphics task the main thread is running */
static OSTask *sYielded;     /* a graphics task yielded to an audio task, until the scheduler starts it again */
static enum { YIELD_QUEUED, YIELD_DRAWING, YIELD_DRAWN } sYieldState;

void Port_RcpInit(void) {
    sLock = SDL_CreateMutex();
    sCond = SDL_CreateCondition();
}

static void enqueue(OSTask *task) {
    if (sCount == MAX_TASKS) {
        Port_Fatal("RCP task queue full");
    }
    sQueue[(sHead + sCount) % MAX_TASKS] = task;
    sQueuedNs[(sHead + sCount) % MAX_TASKS] = SDL_GetTicksNS();
    sCount++;
    SDL_SignalCondition(sCond);
}

void Port_RcpSubmit(OSTask *task) {
    int done = 0;

    if (task->t.type == M_AUDTASK) {
        Port_AudioRunTask(task);
        Port_PostEvent(OS_EVENT_SP);
        return;
    }
    SDL_LockMutex(sLock);
    if (task == sYielded) {
        /* Started again after its yield */
        sYielded = NULL;
        if (sYieldState == YIELD_DRAWN) {
            done = 1;
        } else if (sYieldState == YIELD_QUEUED) {
            enqueue(task);
        }
        /* YIELD_DRAWING: still being drawn; its events come when that is done */
    } else {
        enqueue(task);
    }
    SDL_UnlockMutex(sLock);
    if (done) {
        Port_PostEvent(OS_EVENT_SP);
        Port_PostEvent(OS_EVENT_DP);
    }
}

/* osSpTaskYield: let the audio task through. The SP interrupt reports the yield. */
void Port_RcpYield(void) {
    int yielded = 0;

    SDL_LockMutex(sLock);
    if (sYielded == NULL) {
        if (sDrawing != NULL) {
            sYielded = sDrawing;
            sYieldState = YIELD_DRAWING;
            yielded = 1;
        } else if (sCount > 0) {
            sYielded = sQueue[sHead];
            sHead = (sHead + 1) % MAX_TASKS;
            sCount--;
            sYieldState = YIELD_QUEUED;
            yielded = 1;
        }
    }
    SDL_UnlockMutex(sLock);
    if (yielded) {
        Port_PostEvent(OS_EVENT_SP);
    }
}

/* osSpTaskYielded: did this task yield (rather than finish)? */
int Port_RcpYielded(OSTask *task) {
    int yielded;

    SDL_LockMutex(sLock);
    yielded = task == sYielded;
    SDL_UnlockMutex(sLock);
    return yielded;
}

/* Wake the main thread (a retrace, from the VI clock thread) */
void Port_RcpWake(void) {
    SDL_LockMutex(sLock);
    SDL_SignalCondition(sCond);
    SDL_UnlockMutex(sLock);
}

/* Sleep until a task arrives, Port_RcpWake, or untilNs. */
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
    static int sLogTimes = -1;

    if (sLogTimes < 0) {
        sLogTimes = SDL_getenv("DD64_GFXLOG") != NULL;
    }
    for (;;) {
        OSTask *task;
        u64 queued;
        int events = 1;

        SDL_LockMutex(sLock);
        if (sCount == 0) {
            SDL_UnlockMutex(sLock);
            return;
        }
        task = sQueue[sHead];
        queued = sQueuedNs[sHead];
        sHead = (sHead + 1) % MAX_TASKS;
        sCount--;
        sDrawing = task;
        SDL_UnlockMutex(sLock);

        if (task->t.type == M_GFXTASK) {
            u64 start = SDL_GetTicksNS();
            Port_VideoRunTask(task);
            if (sLogTimes) {
                static u64 waited, ran;
                static unsigned n;
                waited += start - queued;
                ran += SDL_GetTicksNS() - start;
                if (++n == 120) {
                    Port_Log("rcp: graphics tasks waited %.2f ms on average to start, took %.2f ms",
                             waited / 120 / 1e6, ran / 120 / 1e6);
                    waited = ran = 0;
                    n = 0;
                }
            }
        } else {
            Port_Log("RCP: unknown task type %u", (unsigned)task->t.type);
        }

        SDL_LockMutex(sLock);
        sDrawing = NULL;
        if (task == sYielded && sYieldState == YIELD_DRAWING) {
            sYieldState = YIELD_DRAWN; /* the scheduler owes it a restart; the events come then */
            events = 0;
        }
        SDL_UnlockMutex(sLock);
        if (events) {
            Port_PostEvent(OS_EVENT_SP);
            Port_PostEvent(OS_EVENT_DP);
        }
    }
}
