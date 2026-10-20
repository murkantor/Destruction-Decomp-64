/*
 * DD64_PROFILE=path (testing, Linux): a sampling profiler. A profiling timer
 * interrupts whichever thread is using the CPU ~2000 times a second and notes
 * where it was; at exit the addresses and their counts go to path, most
 * frequent first (tools: addr2line -f -e DestructionDerby64 to name them).
 */
#if defined(__linux__)
#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ucontext.h>

#define SLOTS (1 << 16)

static unsigned long sAddr[SLOTS];
static unsigned sCount[SLOTS];
static unsigned sLost;
static const char *sPath;

static void onSample(int sig, siginfo_t *info, void *ctx) {
    unsigned long pc = (unsigned long)((ucontext_t *)ctx)->uc_mcontext.gregs[REG_EIP];
    unsigned h = (unsigned)(pc * 2654435761u) & (SLOTS - 1), n;

    (void)sig;
    (void)info;
    for (n = 0; n < 64; n++, h = (h + 1) & (SLOTS - 1)) {
        if (sAddr[h] == pc || sAddr[h] == 0) {
            sAddr[h] = pc;
            sCount[h]++;
            return;
        }
    }
    sLost++;
}

static int byCount(const void *a, const void *b) {
    unsigned ca = sCount[*(const int *)a], cb = sCount[*(const int *)b];
    return ca < cb ? 1 : ca > cb ? -1 : 0;
}

static void writeProfile(void) {
    static int idx[SLOTS];
    struct itimerval off = { { 0, 0 }, { 0, 0 } };
    FILE *f;
    int i, n = 0;

    setitimer(ITIMER_PROF, &off, NULL);
    for (i = 0; i < SLOTS; i++) {
        if (sCount[i] != 0) {
            idx[n++] = i;
        }
    }
    qsort(idx, n, sizeof(idx[0]), byCount);
    f = fopen(sPath, "w");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < n; i++) {
        fprintf(f, "%lx %u\n", sAddr[idx[i]], sCount[idx[i]]);
    }
    if (sLost) {
        fprintf(f, "0 %u\n", sLost);
    }
    fclose(f);
}

void Port_ProfileStart(void) {
    struct sigaction sa;
    struct itimerval every = { { 0, 500 }, { 0, 500 } };

    sPath = getenv("DD64_PROFILE");
    if (sPath == NULL) {
        return;
    }
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = onSample;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGPROF, &sa, NULL);
    setitimer(ITIMER_PROF, &every, NULL);
    atexit(writeProfile);
}
#else
void Port_ProfileStart(void) {
}
#endif
