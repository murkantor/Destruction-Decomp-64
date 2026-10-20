/*
 * libultra threads, message queues, events and timers.
 *
 * Each N64 thread runs on its own host thread, but only one at a time holds the
 * CPU (sRunning), as on the N64's single core: the highest-priority runnable
 * thread, first come first served within a priority. A thread gives the CPU up
 * when it blocks in osRecvMesg/osSendMesg, stops, or wakes a higher-priority
 * thread.
 *
 * Interrupts (VI retrace, task completion, timers) arrive from host threads as
 * messages: the VI clock (os_hw.c), the main thread (RCP tasks) and the timer
 * clock here. When one wakes a thread with a higher priority than the running
 * one, the switch happens at the running thread's next OS call (any function
 * here, and the cache, timer and DMA calls in os_hw.c). If the running thread
 * makes none for PREEMPT_GRACE_NS (a busy loop), the timer clock hands the CPU
 * over anyway; the old thread then stops at its next OS call.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"

#define MAX_THREADS 32
#define THREAD_STACK_SIZE (4 * 1024 * 1024)
#define PREEMPT_GRACE_NS (4 * 1000 * 1000)
#define MAX_TIMERS 64

typedef struct PortThread {
    OSThread *t;
    SDL_Thread *host;
    SDL_Condition *cond;
    void (*entry)(void *);
    void *arg;
    OSMesgQueue *waitRecv;
    OSMesgQueue *waitSend;
    u32 seq;
    int ready;
} PortThread;

static SDL_Mutex *sLock;
static PortThread sThreads[MAX_THREADS];
static int sNumThreads;
static OSThread *sRunning;
static u32 sSeq;
static u64 sPreemptSince;
static SDL_TLSID sSelfTls;

static struct {
    OSMesgQueue *mq;
    OSMesg msg;
} sEvents[OS_NUM_EVENTS];

static OSTimer *sTimers[MAX_TIMERS];
static int sNumTimers;
static SDL_Condition *sTimerCond; /* wakes the timer clock for a new timer */

static u64 sTimeBaseNs;

static int timerClock(void *data);

static PortThread *self(void) {
    return SDL_GetTLS(&sSelfTls);
}

static PortThread *find(OSThread *t) {
    int i;

    for (i = 0; i < sNumThreads; i++) {
        if (sThreads[i].t == t) {
            return &sThreads[i];
        }
    }
    return NULL;
}

static PortThread *pick(void) {
    PortThread *best = NULL;
    int i;

    for (i = 0; i < sNumThreads; i++) {
        PortThread *p = &sThreads[i];
        if (!p->ready) {
            continue;
        }
        if (best == NULL || p->t->priority > best->t->priority ||
            (p->t->priority == best->t->priority && p->seq < best->seq)) {
            best = p;
        }
    }
    return best;
}

static void makeReady(PortThread *p) {
    p->ready = 1;
    p->seq = ++sSeq;
    p->t->state = OS_STATE_RUNNABLE;
}

/* Give the CPU to the best ready thread, or to nobody. */
static void dispatch(void) {
    PortThread *n = pick();

    sPreemptSince = 0;
    if (n == NULL) {
        sRunning = NULL;
        return;
    }
    n->ready = 0;
    n->t->state = OS_STATE_RUNNING;
    sRunning = n->t;
    SDL_SignalCondition(n->cond);
}

static void waitTurn(PortThread *p) {
    while (sRunning != p->t) {
        SDL_WaitCondition(p->cond, sLock);
    }
}

/* Lock, and if the watchdog took the CPU away from this thread, wait for it back. */
static PortThread *enter(void) {
    PortThread *p;

    SDL_LockMutex(sLock);
    p = self();
    if (p != NULL && sRunning != p->t) {
        waitTurn(p);
    }
    return p;
}

static void leave(void) {
    SDL_UnlockMutex(sLock);
}

static void block(PortThread *p) {
    p->t->state = OS_STATE_WAITING;
    dispatch();
    waitTurn(p);
}

static void yieldTo(PortThread *p) {
    makeReady(p);
    dispatch();
    waitTurn(p);
}

/* After making threads ready: switch now if one outranks the running thread. */
static void reschedule(PortThread *p) {
    PortThread *n = pick();

    if (n == NULL) {
        return;
    }
    if (sRunning == NULL) {
        dispatch();
    } else if (p != NULL && sRunning == p->t) {
        if (n->t->priority > p->t->priority) {
            yieldTo(p);
        }
    } else if (n->t->priority > sRunning->priority && sPreemptSince == 0) {
        /* From the host: the running thread switches at its next OS call. */
        sPreemptSince = SDL_GetTicksNS();
        SDL_SignalCondition(sTimerCond); /* the watchdog's deadline for the timer clock */
    }
}

static void wake(OSMesgQueue *mq, int forRecv) {
    PortThread *best = NULL;
    int i;

    for (i = 0; i < sNumThreads; i++) {
        PortThread *p = &sThreads[i];
        if ((forRecv ? p->waitRecv : p->waitSend) == mq &&
            (best == NULL || p->t->priority > best->t->priority)) {
            best = p;
        }
    }
    if (best != NULL) {
        best->waitRecv = NULL;
        best->waitSend = NULL;
        makeReady(best);
    }
}

static int sendLocked(PortThread *p, OSMesgQueue *mq, OSMesg msg, s32 flag, int jam) {
    while (mq->validCount >= mq->msgCount) {
        if (flag == OS_MESG_NOBLOCK || p == NULL) {
            return -1;
        }
        p->waitSend = mq;
        block(p);
    }
    if (jam) {
        mq->first = (mq->first + mq->msgCount - 1) % mq->msgCount;
        mq->msg[mq->first] = msg;
    } else {
        mq->msg[(mq->first + mq->validCount) % mq->msgCount] = msg;
    }
    mq->validCount++;
    wake(mq, 1);
    return 0;
}

static int threadMain(void *data) {
    PortThread *p = data;

    SDL_SetTLS(&sSelfTls, p, NULL);
    SDL_LockMutex(sLock);
    waitTurn(p);
    SDL_UnlockMutex(sLock);

    p->entry(p->arg);

    /* Returning from the entry point ends the thread, like __osCleanupThread. */
    SDL_LockMutex(sLock);
    p->t->state = OS_STATE_STOPPED;
    p->ready = 0;
    if (sRunning == p->t) {
        dispatch();
    }
    SDL_UnlockMutex(sLock);
    return 0;
}

void Port_OsInit(void) {
    sLock = SDL_CreateMutex();
    sTimeBaseNs = SDL_GetTicksNS();
    sTimerCond = SDL_CreateCondition();
    if (SDL_CreateThread(timerClock, "timer clock", NULL) == NULL) {
        Port_Fatal("timer clock thread: %s", SDL_GetError());
    }
}

/* ---- Threads ---- */

void osCreateThread(OSThread *t, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri) {
    PortThread *p;

    SDL_LockMutex(sLock);
    p = find(t);
    if (p == NULL) {
        if (sNumThreads == MAX_THREADS) {
            Port_Fatal("osCreateThread: too many threads");
        }
        p = &sThreads[sNumThreads++];
        memset(p, 0, sizeof(*p));
        p->t = t;
        p->cond = SDL_CreateCondition();
    }
    p->entry = entry;
    p->arg = arg;
    t->next = NULL;
    t->queue = NULL;
    t->tlnext = NULL;
    t->id = id;
    t->priority = pri;
    t->state = OS_STATE_STOPPED;
    t->flags = 0;
    SDL_UnlockMutex(sLock);
}

void osStartThread(OSThread *t) {
    PortThread *me = enter();
    PortThread *p = find(t);

    if (p == NULL) {
        leave();
        return;
    }
    if (t->state == OS_STATE_STOPPED || t->state == OS_STATE_WAITING) {
        p->waitRecv = NULL;
        p->waitSend = NULL;
        makeReady(p);
    }
    if (p->host == NULL) {
        SDL_PropertiesID props = SDL_CreateProperties();
        char name[32];

        SDL_snprintf(name, sizeof(name), "n64 thread %d", (int)t->id);
        SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, (void *)threadMain);
        SDL_SetStringProperty(props, SDL_PROP_THREAD_CREATE_NAME_STRING, name);
        SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, p);
        SDL_SetNumberProperty(props, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, THREAD_STACK_SIZE);
        p->host = SDL_CreateThreadWithProperties(props);
        SDL_DestroyProperties(props);
        if (p->host == NULL) {
            Port_Fatal("osStartThread: %s", SDL_GetError());
        }
    }
    reschedule(me);
    leave();
}

void osStopThread(OSThread *t) {
    PortThread *me = enter();
    PortThread *p = t == NULL ? me : find(t);

    if (p != NULL) {
        p->ready = 0;
        p->waitRecv = NULL;
        p->waitSend = NULL;
        p->t->state = OS_STATE_STOPPED;
        if (p == me) {
            dispatch();
            waitTurn(p);
        }
    }
    leave();
}

void osDestroyThread(OSThread *t) {
    osStopThread(t);
}

void osSetThreadPri(OSThread *t, OSPri pri) {
    PortThread *me = enter();
    PortThread *p = t == NULL ? me : find(t);

    if (p != NULL) {
        p->t->priority = pri;
    }
    reschedule(me);
    if (me != NULL) {
        /* Lowering our own priority below a ready thread gives up the CPU. */
        PortThread *n = pick();
        if (n != NULL && n->t->priority > me->t->priority) {
            yieldTo(me);
        }
    }
    leave();
}

OSPri osGetThreadPri(OSThread *t) {
    PortThread *me = self();

    if (t == NULL) {
        return me != NULL ? me->t->priority : 0;
    }
    return t->priority;
}

OSId osGetThreadId(OSThread *t) {
    PortThread *me = self();

    if (t == NULL) {
        return me != NULL ? me->t->id : 0;
    }
    return t->id;
}

void osYieldThread(void) {
    PortThread *me = enter();

    if (me != NULL) {
        yieldTo(me);
    }
    leave();
}

/* ---- Message queues ---- */

void osCreateMesgQueue(OSMesgQueue *mq, OSMesg *msg, s32 count) {
    mq->mtqueue = NULL;
    mq->fullqueue = NULL;
    mq->validCount = 0;
    mq->first = 0;
    mq->msgCount = count;
    mq->msg = msg;
}

s32 osSendMesg(OSMesgQueue *mq, OSMesg msg, s32 flag) {
    PortThread *me = enter();
    s32 ret = sendLocked(me, mq, msg, flag, 0);

    reschedule(me);
    leave();
    return ret;
}

s32 osJamMesg(OSMesgQueue *mq, OSMesg msg, s32 flag) {
    PortThread *me = enter();
    s32 ret = sendLocked(me, mq, msg, flag, 1);

    reschedule(me);
    leave();
    return ret;
}

s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag) {
    PortThread *me = enter();

    while (mq->validCount == 0) {
        if (flag == OS_MESG_NOBLOCK || me == NULL) {
            leave();
            return -1;
        }
        me->waitRecv = mq;
        block(me);
    }
    if (msg != NULL) {
        *msg = mq->msg[mq->first];
    }
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;
    wake(mq, 0);
    reschedule(me);
    leave();
    return 0;
}

/* ---- Events ---- */

void osSetEventMesg(OSEvent event, OSMesgQueue *mq, OSMesg msg) {
    SDL_LockMutex(sLock);
    if (event >= 0 && event < OS_NUM_EVENTS) {
        sEvents[event].mq = mq;
        sEvents[event].msg = msg;
    }
    SDL_UnlockMutex(sLock);
}

/* Raise an interrupt: send its message without blocking, from any thread. */
void Port_PostEvent(OSEvent event) {
    PortThread *me = enter();

    if (sEvents[event].mq != NULL) {
        sendLocked(NULL, sEvents[event].mq, sEvents[event].msg, OS_MESG_NOBLOCK, 0);
        reschedule(me);
    }
    leave();
}

void Port_PostMesg(OSMesgQueue *mq, OSMesg msg) {
    PortThread *me = enter();

    sendLocked(NULL, mq, msg, OS_MESG_NOBLOCK, 0);
    reschedule(me);
    leave();
}

/* Called from OS functions the game uses often, so waiting interrupts get serviced. */
void Port_SafePoint(void) {
    PortThread *me = self();

    if (me == NULL || (sPreemptSince == 0 && sRunning == me->t)) {
        return;
    }
    enter();
    reschedule(me);
    leave();
}

/* ---- Time and timers ---- */

OSTime Port_GetTime(void) {
    u64 ns = SDL_GetTicksNS() - sTimeBaseNs;

    /* ns * 46.875 MHz / 1e9 = ns * 3 / 64 */
    return ns * 3 / 64;
}

OSTime osGetTime(void) {
    Port_SafePoint();
    return Port_GetTime();
}

u32 osGetCount(void) {
    Port_SafePoint();
    return (u32)Port_GetTime();
}

s32 osSetTimer(OSTimer *timer, OSTime countdown, OSTime interval, OSMesgQueue *mq, OSMesg msg) {
    int i;

    SDL_LockMutex(sLock);
    timer->interval = interval;
    timer->value = Port_GetTime() + (countdown != 0 ? countdown : interval);
    timer->mq = mq;
    timer->msg = msg;
    for (i = 0; i < sNumTimers; i++) {
        if (sTimers[i] == timer) {
            break;
        }
    }
    if (i == sNumTimers) {
        if (sNumTimers == MAX_TIMERS) {
            Port_Fatal("osSetTimer: too many timers");
        }
        sTimers[sNumTimers++] = timer;
    }
    SDL_SignalCondition(sTimerCond);
    SDL_UnlockMutex(sLock);
    return 0;
}

/*
 * Block the calling game thread until the game clock (osGetTime in ns, as the
 * game converts it) reaches ns, letting the other game threads run meanwhile.
 * Called from one game thread at a time (the main loop's frame pacing).
 */
void Port_WaitUntilNs(u64 ns) {
    static OSMesgQueue mq;
    static OSMesg buf[1];
    static OSTimer timer;
    OSTime target = ns * 3 / 64, now = Port_GetTime();

    if (target <= now) {
        return;
    }
    osCreateMesgQueue(&mq, buf, 1);
    osSetTimer(&timer, target - now, 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);
}

s32 osStopTimer(OSTimer *timer) {
    int i;

    SDL_LockMutex(sLock);
    for (i = 0; i < sNumTimers; i++) {
        if (sTimers[i] == timer) {
            sTimers[i] = sTimers[--sNumTimers];
            SDL_UnlockMutex(sLock);
            return 0;
        }
    }
    SDL_UnlockMutex(sLock);
    return -1;
}

/*
 * With sLock held: fire the timers that are due, and take the CPU from a thread
 * that has kept a higher-priority one waiting too long. Returns the time the
 * next timer is due (0: none).
 */
static OSTime fireTimers(void) {
    OSTime now = Port_GetTime(), next = 0;
    int i;

    for (i = 0; i < sNumTimers;) {
        OSTimer *t = sTimers[i];
        if (now >= t->value) {
            if (t->mq != NULL) {
                sendLocked(NULL, t->mq, t->msg, OS_MESG_NOBLOCK, 0);
            }
            if (t->interval != 0) {
                t->value += t->interval;
            } else {
                sTimers[i] = sTimers[--sNumTimers];
                continue;
            }
        }
        if (next == 0 || t->value < next) {
            next = t->value;
        }
        i++;
    }
    reschedule(NULL);
    if (sPreemptSince != 0 && SDL_GetTicksNS() - sPreemptSince > PREEMPT_GRACE_NS && sRunning != NULL) {
        PortThread *cur = find(sRunning);
        makeReady(cur);
        dispatch();
    }
    return next;
}

/*
 * Wait until SDL_GetTicksNS() reaches `until`, precisely but mostly asleep: one
 * sleep to `*margin` before it, then a spin. The margin is learnt per caller from
 * how late its sleeps wake (raised at once, lowered slowly). SDL_DelayPrecise
 * naps in 1 ms steps and spins the last 1-2 ms (all of a wait under 1 ms), which
 * kept the 60 Hz clocks spinning several percent of a core: battery, on a laptop
 * or a Steam Deck.
 */
void Port_SleepUntilNs(u64 until, PortSleeper *sl) {
    u64 *margin = &sl->margin;
    u64 now = SDL_GetTicksNS();

    sl->waits++;

    if (*margin == 0) {
        *margin = 1000000;
    }
    if (now + *margin < until) {
        u64 wake = until - *margin, late, need;

        SDL_DelayNS(wake - now);
        now = SDL_GetTicksNS();
        late = now > wake ? now - wake : 0;
        /* aim at how late a sleep usually wakes, not the worst: an odd late wake
           (Windows overshoots by up to ~0.6 ms now and then) only makes that one
           deadline a little late, where sizing for it would spin that long every time */
        need = late + 50000;
        if (need > *margin) {
            *margin += (need - *margin) / 8;
        } else {
            *margin -= (*margin - need) / 16;
        }
        if (*margin < 100000) {
            *margin = 100000;
        } else if (*margin > 2000000) {
            *margin = 2000000;
        }
    }
    {
        u64 t0 = now;
        while (now < until) {
            SDL_CPUPauseInstruction();
            now = SDL_GetTicksNS();
        }
        sl->spunNs += now - t0;
    }
}

/*
 * The timer clock: fires each timer when it is due, on its own host thread like
 * the VI clock, so a timer (the frame pacing's wait, say) is not held up while
 * the main thread draws or waits for vsync. It sleeps until the next timer or the
 * watchdog's deadline (osSetTimer and a preemption wake it to look again), the
 * last stretch precisely.
 */
static int timerClock(void *data) {
    PortSleeper sl = { 0 };
    const int log = SDL_getenv("DD64_GFXLOG") != NULL;
    u64 logAt = SDL_GetTicksNS() + 10000000000ull;
    unsigned condWaits = 0;

    (void)data;
    SDL_LockMutex(sLock);
    while (!gPortQuit) {
        OSTime next = fireTimers(), now = Port_GetTime();
        u64 waitNs = 100000000; /* nothing due: look again in 100 ms anyway */

        if (log && SDL_GetTicksNS() > logAt) {
            Port_Log("timers: per second %.0f precise waits (spinning %.0f us of them), %.0f other waits",
                     sl.waits / 10.0, sl.spunNs / 1e3 / 10.0, condWaits / 10.0);
            sl.waits = 0;
            sl.spunNs = 0;
            condWaits = 0;
            logAt += 10000000000ull;
        }

        if (next != 0) {
            u64 ns = next > now ? (u64)(next - now) * 64 / 3 : 0;
            if (ns < waitNs) {
                waitNs = ns;
            }
        }
        if (sPreemptSince != 0) {
            u64 since = SDL_GetTicksNS() - sPreemptSince;
            u64 left = since < PREEMPT_GRACE_NS ? PREEMPT_GRACE_NS - since : 0;
            if (left < waitNs) {
                waitNs = left;
            }
        }
        if (waitNs < 2000000) {
            SDL_UnlockMutex(sLock);
            Port_SleepUntilNs(SDL_GetTicksNS() + waitNs, &sl);
            SDL_LockMutex(sLock);
        } else {
            /* a millisecond or so early (the wait's granularity), then precisely */
            condWaits++;
            SDL_WaitConditionTimeout(sTimerCond, sLock, (Sint32)(waitNs / 1000000) - 1);
        }
    }
    SDL_UnlockMutex(sLock);
    return 0;
}

/* ---- Interrupt masks: the switch points above already serialise everything ---- */

OSIntMask osSetIntMask(OSIntMask mask) {
    return 0x3FFF01;
}

u32 __osDisableInt(void) {
    return 1;
}

void __osRestoreInt(u32 mask) {
}
