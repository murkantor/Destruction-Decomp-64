/*
 * DD64_PROFILE=path (testing, Linux and Windows): a sampling profiler. A profiling timer
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
#elif defined(_WIN32)
/*
 * Windows: a sampling thread (spinning on one core while it runs) suspends each of
 * the process's other threads ~1300 times a second, at random moments, if it used
 * the CPU since the last look, and notes where it was;
 * one found at the return of a system call (waiting, or in the kernel) is only
 * counted. For an address outside the executable, the function of the executable
 * that led there is noted too (a guess from the stack). Addresses outside it
 * are written with their module's name (the GL driver, the C runtime...).
 */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLOTS (1 << 16)
#define MAX_THREADS 64

static DWORD sAddr[SLOTS];
static unsigned sCount[SLOTS];
static unsigned sLost, sSamples;
static const char *sPath;
static volatile LONG sStop;
static HANDLE sSampler;

/* per thread: its name, the cycles it used, and those in ntdll */
static char sThreadName[MAX_THREADS][64];
static ULONG64 sThreadUsed[MAX_THREADS];
static unsigned sThreadRun[MAX_THREADS], sThreadWait[MAX_THREADS];
static int sThreads;
static HMODULE sNtdll;

static void nameThread(int i, HANDLE h, DWORD id) {
    typedef HRESULT(WINAPI * GetDesc)(HANDLE, PWSTR *);
    static GetDesc getDesc;
    PWSTR w = NULL;

    if (getDesc == NULL) {
        getDesc = (GetDesc)(void (*)(void))GetProcAddress(GetModuleHandleA("kernelbase.dll"), "GetThreadDescription");
    }
    snprintf(sThreadName[i], sizeof(sThreadName[i]), "thread %lu", (unsigned long)id);
    if (getDesc != NULL && SUCCEEDED(getDesc(h, &w)) && w != NULL) {
        if (w[0] != 0) {
            WideCharToMultiByte(CP_UTF8, 0, w, -1, sThreadName[i], sizeof(sThreadName[i]), NULL, NULL);
        }
        LocalFree(w);
    }
}

/* for samples outside the executable: the nearest return address into it on the
   stack (a guess: the first word on the stack that points into its code) */
static DWORD sCaller[SLOTS];
static unsigned sCallerCount[SLOTS];
static DWORD sTextLo, sTextHi;

static void noteCaller(DWORD esp, unsigned weight) {
    const DWORD *sp = (const DWORD *)(UINT_PTR)esp;
    DWORD pc = 0;
    unsigned h, n;
    MEMORY_BASIC_INFORMATION mi;

    if (VirtualQuery(sp, &mi, sizeof(mi)) == 0) {
        return;
    }
    for (n = 0; n < 512 && (const char *)(sp + n + 1) <= (const char *)mi.BaseAddress + mi.RegionSize; n++) {
        if (sp[n] >= sTextLo && sp[n] < sTextHi) {
            pc = sp[n];
            break;
        }
    }
    h = (unsigned)(pc * 2654435761u) & (SLOTS - 1);
    for (n = 0; n < 64; n++, h = (h + 1) & (SLOTS - 1)) {
        if (sCaller[h] == pc || sCallerCount[h] == 0) {
            sCaller[h] = pc;
            sCallerCount[h] += weight;
            return;
        }
    }
}

static void note(DWORD pc, unsigned weight) {
    unsigned h = (unsigned)(pc * 2654435761u) & (SLOTS - 1), n;

    for (n = 0; n < 64; n++, h = (h + 1) & (SLOTS - 1)) {
        if (sAddr[h] == pc || sAddr[h] == 0) {
            sAddr[h] = pc;
            sCount[h] += weight;
            return;
        }
    }
    sLost += weight;
}

static DWORD WINAPI sampleLoop(LPVOID arg) {
    HANDLE threads[MAX_THREADS];
    DWORD ids[MAX_THREADS];
    ULONG64 cycles[MAX_THREADS];
    int n = 0, i, k;
    DWORD self = GetCurrentThreadId(), pid = GetCurrentProcessId(), lastScan = 0;
    unsigned rng = 12345;

    (void)arg;
    sNtdll = GetModuleHandleA("ntdll.dll");
    {
        /* the executable's code */
        BYTE *base = (BYTE *)GetModuleHandleA(NULL);
        IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);

        sTextLo = (DWORD)(UINT_PTR)base + nt->OptionalHeader.BaseOfCode;
        sTextHi = sTextLo + nt->OptionalHeader.SizeOfCode;
    }
    timeBeginPeriod(1);
    while (!sStop) {
        /* the process's threads, looked for again every 200 ms */
        if (GetTickCount() - lastScan > 200) {
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            THREADENTRY32 te;

            lastScan = GetTickCount();
            te.dwSize = sizeof(te);
            if (snap != INVALID_HANDLE_VALUE && Thread32First(snap, &te)) {
                do {
                    if (te.th32OwnerProcessID != pid || te.th32ThreadID == self) {
                        continue;
                    }
                    for (k = 0; k < n && ids[k] != te.th32ThreadID; k++) {
                    }
                    if (k == n && n < MAX_THREADS) {
                        HANDLE h = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                                              FALSE, te.th32ThreadID);
                        if (h != NULL) {
                            threads[n] = h;
                            ids[n] = te.th32ThreadID;
                            QueryThreadCycleTime(h, &cycles[n]);
                            nameThread(n, h, te.th32ThreadID);
                            n++;
                            sThreads = n;
                        }
                    }
                } while (Thread32Next(snap, &te));
            }
            if (snap != INVALID_HANDLE_VALUE) {
                CloseHandle(snap);
            }
        }
        for (i = 0; i < n; i++) {
            ULONG64 c;
            CONTEXT ctx;

            ULONG64 used;

            if (!QueryThreadCycleTime(threads[i], &c) || c == cycles[i]) {
                continue;
            }
            used = c - cycles[i];
            cycles[i] = c;
            if (SuspendThread(threads[i]) == (DWORD)-1) {
                continue;
            }
            ctx.ContextFlags = CONTEXT_CONTROL;
            if (GetThreadContext(threads[i], &ctx)) {
                /* weighted by the cycles it used since the last look (in thousands): a
                   thread that woke briefly and is waiting again counts for little */
                HMODULE m = NULL;

                sThreadUsed[i] += used;
                GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   (LPCSTR)(UINT_PTR)ctx.Eip, &m);
                if (m != NULL && m == sNtdll && (*(BYTE *)(UINT_PTR)ctx.Eip & 0xFE) == 0xC2) {
                    /* at the return of a system call: waiting (or in the kernel), not
                       running here; the cycles it used went before it */
                    sThreadWait[i]++;
                } else {
                    note(ctx.Eip, 1);
                    sThreadRun[i]++;
                    if (ctx.Eip < sTextLo || ctx.Eip >= sTextHi) {
                        noteCaller(ctx.Esp, 1);
                    }
                }
                sSamples++;
            }
            ResumeThread(threads[i]);
        }
        /* the next look after a random 0.25-1.25 ms, waited out on the performance
           counter: sleeping would wake on the same timer tick as the threads looked
           at, and catch them just woken every time */
        {
            LARGE_INTEGER now, freq;
            LONGLONG until;

            QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&now);
            rng = rng * 1664525u + 1013904223u;
            until = now.QuadPart + freq.QuadPart / 4000 + (LONGLONG)((rng >> 8) % 1000) * freq.QuadPart / 1000000;
            do {
                YieldProcessor();
                QueryPerformanceCounter(&now);
            } while (now.QuadPart < until && !sStop);
        }
    }
    timeEndPeriod(1);
    return 0;
}

static int byCount(const void *a, const void *b) {
    unsigned ca = sCount[*(const int *)a], cb = sCount[*(const int *)b];
    return ca < cb ? 1 : ca > cb ? -1 : 0;
}

/* The name of the export of module m nearest below addr, or NULL */
static const char *nearestExport(HMODULE m, DWORD addr) {
    BYTE *base = (BYTE *)m;
    IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
    IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    IMAGE_EXPORT_DIRECTORY *ex;
    DWORD *names, *funcs, best = 0, i;
    WORD *ords;
    const char *name = NULL;

    if (dir->VirtualAddress == 0) {
        return NULL;
    }
    ex = (IMAGE_EXPORT_DIRECTORY *)(base + dir->VirtualAddress);
    names = (DWORD *)(base + ex->AddressOfNames);
    funcs = (DWORD *)(base + ex->AddressOfFunctions);
    ords = (WORD *)(base + ex->AddressOfNameOrdinals);
    for (i = 0; i < ex->NumberOfNames; i++) {
        DWORD f = (DWORD)(UINT_PTR)base + funcs[ords[i]];
        if (f <= addr && f > best) {
            best = f;
            name = (const char *)(base + names[i]);
        }
    }
    return name;
}

static void writeProfile(void) {
    static int idx[SLOTS];
    HMODULE exe = GetModuleHandleA(NULL);
    FILE *f;
    int i, n = 0;

    InterlockedExchange(&sStop, 1);
    WaitForSingleObject(sSampler, 1000);
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
    fprintf(f, "# %u samples\n", sSamples);
    for (i = 0; i < sThreads; i++) {
        if (sThreadUsed[i] != 0) {
            fprintf(f, "# %-24s %8.0f Mcycles; seen running %u times, in a system call %u\n", sThreadName[i],
                    sThreadUsed[i] / 1e6, sThreadRun[i], sThreadWait[i]);
        }
    }
    for (i = 0; i < n; i++) {
        HMODULE m = NULL;
        char name[MAX_PATH] = "?";
        const char *base;

        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)(UINT_PTR)sAddr[idx[i]], &m);
        if (m == exe) {
            strcpy(name, "exe");
        } else if (m != NULL) {
            GetModuleFileNameA(m, name, sizeof(name));
        }
        base = strrchr(name, '\\');
        base = base != NULL ? base + 1 : name;
        if (m != NULL && m != exe) {
            const char *fn = nearestExport(m, sAddr[idx[i]]);
            fprintf(f, "%lx %u %s!%s\n", (unsigned long)sAddr[idx[i]], sCount[idx[i]], base, fn != NULL ? fn : "?");
        } else {
            fprintf(f, "%lx %u %s\n", (unsigned long)sAddr[idx[i]], sCount[idx[i]], base);
        }
    }
    if (sLost) {
        fprintf(f, "0 %u lost\n", sLost);
    }
    /* the callers in the executable of the samples outside it */
    for (i = 0; i < SLOTS; i++) {
        if (sCallerCount[i] != 0) {
            fprintf(f, "caller %lx %u\n", (unsigned long)sCaller[i], sCallerCount[i]);
        }
    }
    fclose(f);
}

void Port_ProfileStart(void) {
    sPath = getenv("DD64_PROFILE");
    if (sPath == NULL) {
        return;
    }
    sSampler = CreateThread(NULL, 0, sampleLoop, NULL, 0, NULL);
    SetThreadPriority(sSampler, THREAD_PRIORITY_TIME_CRITICAL);
    atexit(writeProfile);
}
#else
void Port_ProfileStart(void) {
}
#endif
