/*
 * raphnet N64-to-USB adapters, through their raw interface: the controller's
 * own answer to the N64 status command, as the console reads it.
 *
 * Ported from Project Midas (port/src/raphnet.c, its GoldenEye PC port), which
 * describes it as a clean-room implementation of the publicly documented wire
 * format (the pj64raphnetraw / gcn64lib protocol; the constants are
 * interoperability facts, no GPL code copied). The protocol, the transport
 * health logic, the poller thread and the Rumble Pak writes are Midas's; the
 * DD64 changes are the interface below, logging, and SDL3.
 *
 * The protocol:
 *  - HID feature reports, report id 0, 63 bytes (firmware 3.4 and later; 40 before).
 *  - Request: [0x80 = raw SI command, channel, tx_len, payload...].
 *  - Reply (get the feature report until its first byte echoes the request):
 *    [0x80, channel, rx_len, data...].
 *  - The N64 status command 0x01 answers 4 bytes: the button word, in the
 *    CONT_* layout (A 0x8000 ... C right 0x0001), and the stick, signed N64
 *    units. Nothing is translated, so the stick reads exactly as on hardware:
 *    no dead zone, no range scaling.
 *  - The adapter polls the controller itself for its joystick interface; in
 *    raw use that polling is suspended (RQ_SUSPEND_POLLING), and resumed at exit.
 *
 * Windows: SDL's hidapi leaves the adapter's vendor interface out of its
 * enumeration (Midas checked: only mi_00 is listed), so the Win32 HID API is used
 * directly (SetupDi enumeration, HidD_SetFeature / HidD_GetFeature). Elsewhere
 * SDL_hid_*, whose hidraw enumeration lists every interface. On Linux the
 * hidraw node needs read/write access for the user, e.g. a udev rule:
 *   SUBSYSTEM=="hidraw", ATTRS{idVendor}=="289b", MODE="0666"
 *
 * One thread owns the device (see the poller below); the game's threads only
 * read the cache it fills, so a USB round trip (several ms) never stalls a frame.
 *
 * Testing aids: DD64_RAPHDBG=1 logs every failed exchange and the poll rate
 * every 10 s; DD64_RAPHNET_SYNC=1 polls on the calling thread instead.
 */
#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#endif
#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdlib.h>

#include "port_raphnet.h"

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;

void Port_Log(const char *fmt, ...);

#define RAPHNET_VID          0x289B
#define RQ_RAW_SI_COMMAND    0x80
#define RQ_SUSPEND_POLLING   0x03
#define N64_GET_STATUS       0x01
#define RAPHNET_REPORT     63   /* 3.4+; pre-3.4 firmware uses 40 */

static int s_ReportSize = RAPHNET_REPORT;
static int s_Tried;

/* Transport health: the handle can go stale (USB re-enumeration, a hub power
 * event, selective suspend, unplugging), after which every exchange fails.
 * After RN_FAIL_LIMIT failures in a row it is closed and re-opened, at most
 * once per RN_RETRY_MS, until the adapter answers again. */
#define RN_FAIL_LIMIT   20
#define RN_RETRY_MS   1000
static int s_Fails;
static int s_Lost;           /* reported the drop, waiting to recover */
static int s_Suspended;      /* the adapter's own polling is suspended */
static unsigned s_NextTry;
static unsigned s_OpenRetries;
static int s_Dbg;

#define RN_TICKS() ((unsigned)SDL_GetTicks())
#define RN_DELAY(ms) SDL_Delay(ms)

/* ---- Windows: native HID feature reports ---- */
#ifdef _WIN32

/* hid.dll (-lhid), declared here rather than through the DDK headers */
typedef struct { ULONG Size; USHORT VendorID, ProductID, VersionNumber; } RN_HIDD_ATTRIBUTES;
__declspec(dllimport) void __stdcall HidD_GetHidGuid(GUID *guid);
__declspec(dllimport) BOOLEAN __stdcall HidD_SetFeature(HANDLE h, PVOID buf, ULONG len);
__declspec(dllimport) BOOLEAN __stdcall HidD_GetFeature(HANDLE h, PVOID buf, ULONG len);

static HANDLE s_Dev = INVALID_HANDLE_VALUE;

static int raphnetSendFeature(const u8 *buf, int len) {
    return HidD_SetFeature(s_Dev, (PVOID)buf, (ULONG)len) ? 0 : -1;
}

static int raphnetGetFeature(u8 *buf, int len) {
    return HidD_GetFeature(s_Dev, (PVOID)buf, (ULONG)len) ? len : -1;
}

static int raphnetIsOpen(void) {
    return s_Dev != INVALID_HANDLE_VALUE;
}

static void raphnetCloseDev(void) {
    if (s_Dev != INVALID_HANDLE_VALUE) {
        CloseHandle(s_Dev);
        s_Dev = INVALID_HANDLE_VALUE;
    }
}

#else /* ---- elsewhere: SDL's hidapi ---- */

static SDL_hid_device *s_Dev;

static int raphnetSendFeature(const u8 *buf, int len) {
    return SDL_hid_send_feature_report(s_Dev, buf, (size_t)len) < 0 ? -1 : 0;
}

static int raphnetGetFeature(u8 *buf, int len) {
    return SDL_hid_get_feature_report(s_Dev, buf, (size_t)len);
}

static int raphnetIsOpen(void) {
    return s_Dev != NULL;
}

static void raphnetCloseDev(void) {
    if (s_Dev != NULL) {
        SDL_hid_close(s_Dev);
        s_Dev = NULL;
    }
}

#endif

/* ---- the protocol ---- */

static unsigned long long s_ExchUs;
static unsigned s_ExchN, s_ExchMaxUs, s_ConfirmN, s_RumbleWrites;
#define RN_NOPAK_RETRY_MS 3000
static unsigned s_NoPakUntil[RAPHNET_MAX_CHAN]; /* an unarmed pak is not retried before this */
static u8 s_NoPakLogged[RAPHNET_MAX_CHAN];

static unsigned long long rnUsec(void) {
    /* split, or count * 1e6 overflows after ~21 days of uptime */
    const unsigned long long q = SDL_GetPerformanceCounter();
    const unsigned long long f = SDL_GetPerformanceFrequency();
    return (q / f) * 1000000ull + (q % f) * 1000000ull / f;
}

static int raphnetExchangeOnceRaw(const u8 *cmd, int cmdlen, u8 *rep, int repmax) {
    u8 buf[RAPHNET_REPORT + 1];
    int tries, n;

    SDL_memset(buf, 0, sizeof(buf));
    SDL_memcpy(buf + 1, cmd, cmdlen <= RAPHNET_REPORT ? cmdlen : RAPHNET_REPORT);
    if (raphnetSendFeature(buf, s_ReportSize + 1) < 0) {
        return -1;
    }
    for (tries = 0; tries < 64; tries++) {
        SDL_memset(buf, 0, sizeof(buf));
        n = raphnetGetFeature(buf, s_ReportSize + 1);
        if (n < 0) {
            return -1;
        }
        if (buf[1] == cmd[0]) {
            n = s_ReportSize < repmax ? s_ReportSize : repmax;
            SDL_memcpy(rep, buf + 1, n);
            return n;
        }
        RN_DELAY(1);
    }
    return -1;
}

static int raphnetExchangeOnce(const u8 *cmd, int cmdlen, u8 *rep, int repmax) {
    const unsigned long long t0 = rnUsec();
    const int n = raphnetExchangeOnceRaw(cmd, cmdlen, rep, repmax);
    const unsigned us = (unsigned)(rnUsec() - t0);

    s_ExchUs += us;
    s_ExchN++;
    if (us > s_ExchMaxUs) {
        s_ExchMaxUs = us;
    }
    return n;
}

/* Does this open interface answer a raw status command? Tries both report sizes. */
static int raphnetProbe(const char *path) {
    u8 cmd[4] = { RQ_RAW_SI_COMMAND, 0, 1, N64_GET_STATUS };
    u8 rep[8];

    s_ReportSize = RAPHNET_REPORT;
    if (raphnetExchangeOnce(cmd, 4, rep, sizeof(rep)) >= 3) {
        Port_Log("raphnet: adapter online, raw N64 polling (%s)", path);
        return 1;
    }
    s_ReportSize = 40;
    if (raphnetExchangeOnce(cmd, 4, rep, sizeof(rep)) >= 3) {
        Port_Log("raphnet: adapter online, raw N64 polling, pre-3.4 firmware (%s)", path);
        return 1;
    }
    /* normal for the joystick interfaces of a multi-interface adapter */
    if (s_OpenRetries <= 1) {
        Port_Log("raphnet: no answer from %s", path);
    }
    return 0;
}

#ifdef _WIN32
static int rnContains(const char *hay, const char *needle) {
    int i, j;
    for (i = 0; hay[i] != '\0'; i++) {
        for (j = 0; needle[j] != '\0' && SDL_tolower((unsigned char)hay[i + j]) == needle[j]; j++) {
        }
        if (needle[j] == '\0') {
            return 1;
        }
    }
    return 0;
}

/* Every raphnet HID interface is tried: the vendor interface is mi_01 on
 * single-port adapters but mi_02 on dual-port ones; the raw answer is the test. */
static int raphnetOpen(void) {
    GUID hidGuid;
    HDEVINFO devs;
    SP_DEVICE_INTERFACE_DATA ifd;
    DWORD index;

    if (s_Tried) {
        return raphnetIsOpen();
    }
    s_Tried = 1;
    HidD_GetHidGuid(&hidGuid);
    devs = SetupDiGetClassDevsA(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devs == INVALID_HANDLE_VALUE) {
        return 0;
    }
    ifd.cbSize = sizeof(ifd);
    for (index = 0; SetupDiEnumDeviceInterfaces(devs, NULL, &hidGuid, index, &ifd); index++) {
        char detailbuf[sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A) + 512];
        SP_DEVICE_INTERFACE_DETAIL_DATA_A *detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_A *)detailbuf;

        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);
        if (!SetupDiGetDeviceInterfaceDetailA(devs, &ifd, detail, sizeof(detailbuf), NULL, NULL)) {
            continue;
        }
        if (!rnContains(detail->DevicePath, "vid_289b")) {
            continue;
        }
        s_Dev = CreateFileA(detail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL, OPEN_EXISTING, 0, NULL);
        if (s_Dev == INVALID_HANDLE_VALUE) {
            if (s_OpenRetries <= 1) {
                Port_Log("raphnet: cannot open %s", detail->DevicePath);
            }
            continue;
        }
        if (raphnetProbe(detail->DevicePath)) {
            break;
        }
        raphnetCloseDev();
    }
    SetupDiDestroyDeviceInfoList(devs);
    return raphnetIsOpen();
}
#else
static int raphnetOpen(void) {
    SDL_hid_device_info *list, *d;

    if (s_Tried) {
        return raphnetIsOpen();
    }
    s_Tried = 1;
    if (SDL_hid_init() != 0) {
        if (s_OpenRetries <= 1) {
            Port_Log("raphnet: SDL_hid_init failed: %s", SDL_GetError());
        }
        return 0;
    }
    list = SDL_hid_enumerate(RAPHNET_VID, 0);
    for (d = list; d != NULL; d = d->next) {
        s_Dev = SDL_hid_open_path(d->path);
        if (s_Dev == NULL) {
            if (s_OpenRetries <= 1) {
                Port_Log("raphnet: cannot open %s (%s): the hidraw node needs read/write access, "
                         "see the udev rule in port/src/raphnet.c", d->path, SDL_GetError());
            }
            continue;
        }
        if (raphnetProbe(d->path)) {
            break;
        }
        raphnetCloseDev();
    }
    SDL_hid_free_enumeration(list);
    return raphnetIsOpen();
}
#endif

/* An exchange that re-opens the adapter after a run of failures. */
static int raphnetExchange(const u8 *cmd, int cmdlen, u8 *rep, int repmax) {
    int n = raphnetExchangeOnce(cmd, cmdlen, rep, repmax);
    unsigned now;

    if (n >= 0) {
        if (s_Lost) {
            Port_Log("raphnet: adapter recovered after %d failed exchanges", s_Fails);
            s_Lost = 0;
        }
        s_Fails = 0;
        return n;
    }
    s_Fails++;
    if (s_Dbg) {
        Port_Log("raphnet: exchange failed (cmd %02x, %d in a row)", cmd[0], s_Fails);
    }
    if (s_Fails < RN_FAIL_LIMIT) {
        return -1;
    }
    now = RN_TICKS();
    if (!s_Lost) {
        Port_Log("raphnet: adapter stopped responding; reconnecting");
        s_Lost = 1;
    }
    if (s_NextTry != 0 && (int)(now - s_NextTry) < 0) {
        return -1;
    }
    s_NextTry = now + RN_RETRY_MS;
    raphnetCloseDev();
    s_Suspended = 0; /* a re-opened adapter starts unsuspended */
    s_Tried = 0;
    if (!raphnetOpen()) {
        return -1;
    }
    s_Fails = 0;
    n = raphnetExchangeOnce(cmd, cmdlen, rep, repmax);
    if (n >= 0 && s_Lost) {
        Port_Log("raphnet: adapter recovered");
        s_Lost = 0;
    }
    return n;
}

/* raphnetOpen, retried at most once per RN_RETRY_MS while closed, so an
 * adapter plugged in (or back in) later is picked up. */
static int raphnetOpenRetry(void) {
    if (s_Tried && !raphnetIsOpen()) {
        const unsigned now = RN_TICKS();

        if (s_NextTry != 0 && (int)(now - s_NextTry) < 0) {
            return 0;
        }
        s_NextTry = now + RN_RETRY_MS;
        s_Suspended = 0;
        s_Tried = 0;
        s_OpenRetries++;
        if (!raphnetOpen()) {
            return 0;
        }
        s_OpenRetries = 0;
        return 1;
    }
    if (!raphnetOpen()) {
        s_NextTry = RN_TICKS() + RN_RETRY_MS;
        return 0;
    }
    return 1;
}

/* The adapter's own polling (for its joystick interface) includes pak
 * management, which undoes raw pak writes, so it is suspended for the session
 * and resumed at exit, leaving the adapter working as a joystick again. */
static void raphnetResume(void) {
    if (s_Suspended && raphnetIsOpen()) {
        u8 cmd[2] = { RQ_SUSPEND_POLLING, 0 };
        u8 rep[8];
        raphnetExchange(cmd, 2, rep, sizeof(rep));
        s_Suspended = 0;
    }
}

static void raphnetSuspendOnce(void) {
    if (!s_Suspended && raphnetIsOpen()) {
        u8 cmd[2] = { RQ_SUSPEND_POLLING, 1 };
        u8 rep[8];
        if (raphnetExchange(cmd, 2, rep, sizeof(rep)) >= 0) {
            s_Suspended = 1;
            Port_Log("raphnet: adapter polling suspended (raw session)");
        }
    }
}

/* The controller on a channel: 1 with its state, 0 if none answers, -1 on an adapter error. */
static s32 raphnetPollHw(s32 chan, u16 *btn, s8 *x, s8 *y);
static s32 raphnetRumbleHw(s32 chan, s32 on);

/* ---- the poller ----
 *
 * An exchange is a USB control round trip (a feature report out, then reads
 * until the answer is there), several milliseconds whatever the code does, so
 * it runs on its own thread. The thread never spins (it blocks in the USB call
 * or sleeps), and there is no mutex: a sample is 4 bytes (button word, stick x,
 * stick y), packed into one aligned 32-bit word that is stored and loaded
 * whole; the button word's unused bits 0x40 and 0x80 carry present and error.
 * Rumble is a desired state, not a queue, so it cannot back up. Only this
 * thread touches the device once it runs. */
#define RN_CACHE_PRESENT 0x0040u
#define RN_CACHE_ERROR   0x0080u
#define RN_HOT_MS        2000 /* a channel read this recently is polled every pass */
#define RN_IDLE_POLL_MS   250 /* others this often */
#define RN_RATE_LOG_MS  10000
#define RN_RUMBLE_MIN_MS  100

static SDL_AtomicU32 s_Cache[RAPHNET_MAX_CHAN];
static SDL_AtomicU32 s_AskMs[RAPHNET_MAX_CHAN]; /* when the game last read the channel */
static SDL_AtomicInt s_RumbleWant[RAPHNET_MAX_CHAN];
static SDL_AtomicInt s_Stop;
static u8 s_HwPresent[RAPHNET_MAX_CHAN];
static SDL_Thread *s_Thread;
static int s_Sync = 1; /* polling inline: before the thread starts, or DD64_RAPHNET_SYNC */
static int s_Started;

static u32 raphnetPack(s32 status, u16 btn, s8 x, s8 y) {
    u32 w = (u32)(btn & 0xFF3Fu) | ((u32)(u8)x << 16) | ((u32)(u8)y << 24);

    if (status > 0) {
        w |= RN_CACHE_PRESENT;
    } else if (status < 0) {
        w |= RN_CACHE_ERROR;
    }
    return w;
}

static int SDLCALL raphnetPollLoop(void *unused) {
    s32 rumbleHave[RAPHNET_MAX_CHAN];
    unsigned lastPoll[RAPHNET_MAX_CHAN], lastRumble[RAPHNET_MAX_CHAN], polls[RAPHNET_MAX_CHAN];
    unsigned rateMs = RN_TICKS();
    s32 chan;

    for (chan = 0; chan < RAPHNET_MAX_CHAN; chan++) {
        rumbleHave[chan] = -1;
        lastPoll[chan] = 0;
        lastRumble[chan] = rateMs - RN_RUMBLE_MIN_MS;
        polls[chan] = 0;
    }
    while (!SDL_GetAtomicInt(&s_Stop)) {
        int didwork = 0;
        unsigned now = RN_TICKS();

        for (chan = 0; chan < RAPHNET_MAX_CHAN; chan++) {
            u16 btn = 0;
            s8 x = 0, y = 0;

            /* the age is signed: the game may stamp a read after this pass's `now` */
            if ((int)(now - SDL_GetAtomicU32(&s_AskMs[chan])) < RN_HOT_MS ||
                (unsigned)(now - lastPoll[chan]) >= RN_IDLE_POLL_MS) {
                s32 r;
                lastPoll[chan] = now;
                r = raphnetPollHw(chan, &btn, &x, &y);
                SDL_SetAtomicU32(&s_Cache[chan], raphnetPack(r, btn, x, y));
                polls[chan]++;
                didwork = 1;
            }
            /* the motor after the read, at most once per RN_RUMBLE_MIN_MS, so
               rumble never delays a read; skipped pulses coalesce */
            if (SDL_GetAtomicInt(&s_RumbleWant[chan]) != rumbleHave[chan] &&
                (unsigned)(now - lastRumble[chan]) >= RN_RUMBLE_MIN_MS) {
                const s32 want = SDL_GetAtomicInt(&s_RumbleWant[chan]);
                lastRumble[chan] = now;
                if (raphnetRumbleHw(chan, want) == 0) {
                    rumbleHave[chan] = want;
                }
                didwork = 1;
            }
        }
        if (s_Dbg && (unsigned)(now - rateMs) >= RN_RATE_LOG_MS) {
            const unsigned dt = now - rateMs;
            const unsigned avg10 = s_ExchN ? (unsigned)(s_ExchUs * 10 / s_ExchN / 1000) : 0;
            char line[256];
            int len = SDL_snprintf(line, sizeof(line), "raphnet: rate");

            for (chan = 0; chan < RAPHNET_MAX_CHAN; chan++) {
                const u32 w = SDL_GetAtomicU32(&s_Cache[chan]);
                if (len >= 0 && len < (int)sizeof(line)) {
                    len += SDL_snprintf(line + len, sizeof(line) - (size_t)len, " | ch%d %uHz %s", (int)chan,
                                        (polls[chan] * 1000 + dt / 2) / dt,
                                        (w & RN_CACHE_ERROR) ? "error" : (w & RN_CACHE_PRESENT) ? "pad" : "empty");
                }
                polls[chan] = 0;
            }
            if (len >= 0 && len < (int)sizeof(line)) {
                SDL_snprintf(line + len, sizeof(line) - (size_t)len,
                             " | exchange avg %u.%ums max %u.%ums, %u/s, %u confirms, %u rumble writes", avg10 / 10,
                             avg10 % 10, s_ExchMaxUs / 1000, s_ExchMaxUs / 100 % 10,
                             (s_ExchN * 1000 + dt / 2) / dt, s_ConfirmN, s_RumbleWrites);
            }
            Port_Log("%s", line);
            s_ExchUs = 0;
            s_ExchN = s_ExchMaxUs = s_ConfirmN = s_RumbleWrites = 0;
            rateMs = now;
        }
        RN_DELAY(didwork ? 1 : 20);
    }
    raphnetResume();
    return 0;
}

static void raphnetStop(void) {
    if (s_Thread != NULL) {
        SDL_SetAtomicInt(&s_Stop, 1);
        SDL_WaitThread(s_Thread, NULL); /* bounded by one exchange; the thread resumes the adapter */
        s_Thread = NULL;
    } else {
        raphnetResume();
    }
}

/* ---- the interface (port_raphnet.h) ---- */

void Port_RaphnetStart(void) {
    s32 chan;

    if (s_Started) {
        return;
    }
    s_Started = 1;
    s_Dbg = getenv("DD64_RAPHDBG") != NULL;
    /* Before the thread: this thread owns the device. A first answer for both
       channels, so the game's first controller query sees what is plugged in. */
    if (raphnetOpenRetry()) {
        for (chan = 0; chan < RAPHNET_MAX_CHAN; chan++) {
            u16 b = 0;
            s8 x = 0, y = 0;
            s32 r = raphnetPollHw(chan, &b, &x, &y);
            SDL_SetAtomicU32(&s_Cache[chan], raphnetPack(r, b, x, y));
            Port_Log("raphnet: port %d: %s", (int)chan + 1, r > 0 ? "controller" : r == 0 ? "empty" : "error");
        }
    } else {
        Port_Log("raphnet: no adapter found; it is picked up if plugged in later");
    }
    atexit(raphnetStop);
    if (getenv("DD64_RAPHNET_SYNC") != NULL) {
        return;
    }
    s_Thread = SDL_CreateThread(raphnetPollLoop, "raphnet", NULL);
    if (s_Thread == NULL) {
        Port_Log("raphnet: poller thread failed to start (%s); polling inline", SDL_GetError());
        return;
    }
    s_Sync = 0;
}

int Port_RaphnetOnline(void) {
    return s_Started && (raphnetIsOpen() || s_Lost);
}

int Port_RaphnetPoll(int chan, u16 *btn, s8 *x, s8 *y) {
    u32 w;

    if (!s_Started || chan < 0 || chan >= RAPHNET_MAX_CHAN) {
        return -1;
    }
    SDL_SetAtomicU32(&s_AskMs[chan], RN_TICKS()); /* keeps the channel hot */
    if (s_Sync) {
        return raphnetPollHw(chan, btn, x, y);
    }
    w = SDL_GetAtomicU32(&s_Cache[chan]);
    if (btn != NULL) {
        *btn = (u16)(w & 0xFF3Fu);
    }
    if (x != NULL) {
        *x = (s8)((w >> 16) & 0xFF);
    }
    if (y != NULL) {
        *y = (s8)((w >> 24) & 0xFF);
    }
    return (w & RN_CACHE_ERROR) ? -1 : (w & RN_CACHE_PRESENT) ? 1 : 0;
}

int Port_RaphnetPeek(int chan) {
    u32 w;

    if (!s_Started || chan < 0 || chan >= RAPHNET_MAX_CHAN) {
        return -1;
    }
    w = SDL_GetAtomicU32(&s_Cache[chan]);
    return (w & RN_CACHE_ERROR) ? -1 : (w & RN_CACHE_PRESENT) ? 1 : 0;
}

void Port_RaphnetRumble(int chan, int on) {
    if (!s_Started || chan < 0 || chan >= RAPHNET_MAX_CHAN) {
        return;
    }
    if (s_Sync) {
        raphnetRumbleHw(chan, on);
        return;
    }
    SDL_SetAtomicInt(&s_RumbleWant[chan], on != 0);
}

/* ---- the hardware ---- */

static s32 raphnetPollHw(s32 chan, u16 *btn, s8 *x, s8 *y) {
    u8 cmd[4] = { RQ_RAW_SI_COMMAND, 0, 1, N64_GET_STATUS };
    u8 rep[16];
    int n;

    if (!raphnetOpenRetry() || !raphnetIsOpen() || chan < 0 || chan >= RAPHNET_MAX_CHAN) {
        return -1;
    }
    raphnetSuspendOnce();
    cmd[1] = (u8)chan;
    n = raphnetExchange(cmd, 4, rep, sizeof(rep));
    if (n < 3) {
        return -1;
    }
    if (rep[2] != 4) { /* rx_len 0: nothing plugged into that port */
        /* A late Rumble Pak write reply shares the command byte 0x80 and can be
           taken for this poll's answer: a present pad is asked again before it
           counts as unplugged. */
        if (!s_HwPresent[chan]) {
            return 0;
        }
        s_ConfirmN++;
        n = raphnetExchange(cmd, 4, rep, sizeof(rep));
        if (n < 3) {
            return -1;
        }
        if (rep[2] != 4) {
            s_HwPresent[chan] = 0;
            return 0;
        }
    }
    s_HwPresent[chan] = 1;
    if (btn != NULL) {
        *btn = (u16)((rep[3] << 8) | rep[4]) & 0xFF3F; /* without the reset and unused bits */
    }
    if (x != NULL) {
        *x = (s8)rep[5];
    }
    if (y != NULL) {
        *y = (s8)rep[6];
    }
    return 1;
}

/*
 * The Rumble Pak, driven with the pak protocol over raw SI as the console does
 * (libultra's osMotorInit / osMotorStart / osMotorStop): arm it by writing
 * 32 x 0x80 to block 1024 (address 0x8000), then switch the motor with
 * 32 x 0x01 / 32 x 0x00 at block 1536 (0xC000); each is a pak write (0x03)
 * with the 5-bit address CRC. The game pulses the motor itself; only the
 * changes are sent, which is the waveform the pak would get anyway.
 */
static u8 rnPakAddrCrc(u16 addr) {
    u8 temp = 0, temp2;
    int i;

    for (i = 0; i < 16; i++) {
        temp2 = (temp & 0x10) ? 21 : 0;
        temp <<= 1;
        temp |= (u8)((addr & 0x400) ? 1 : 0);
        addr <<= 1;
        temp ^= temp2;
    }
    return temp & 0x1f;
}

/* The pak data CRC of 32 bytes of `fill` (libultra's __osContDataCrc) */
static u8 rnPakDataCrc(u8 fill) {
    u8 temp = 0, temp2;
    int i, j;

    for (i = 0; i <= 32; i++) {
        for (j = 7; j >= 0; j--) {
            temp2 = (temp & 0x80) ? 133 : 0;
            temp <<= 1;
            if (i < 32) {
                temp |= (u8)((fill & (1 << j)) ? 1 : 0);
            }
            temp ^= temp2;
        }
    }
    return temp;
}

static s32 raphnetPakWrite(s32 chan, u16 block, u8 fill) {
    u8 cmd[3 + 35];
    u8 rep[8];
    u16 af = (u16)((block << 5) | rnPakAddrCrc(block));
    int i, n;

    cmd[0] = RQ_RAW_SI_COMMAND;
    cmd[1] = (u8)chan;
    cmd[2] = 35;   /* tx: command, address (2), data (32) */
    cmd[3] = 0x03; /* pak write */
    cmd[4] = (u8)(af >> 8);
    cmd[5] = (u8)(af & 0xFF);
    for (i = 0; i < 32; i++) {
        cmd[6 + i] = fill;
    }
    n = raphnetExchange(cmd, sizeof(cmd), rep, sizeof(rep));
    if (n < 4) {
        return -1;
    }
    /* The controller answers with the data CRC whether a pak is in or not;
       the CRC inverted means an empty slot. */
    if (rep[2] == 1 && rep[3] == (u8)~rnPakDataCrc(fill)) {
        return -1;
    }
    return rep[2] == 1 ? 0 : -1;
}

static s32 raphnetRumbleHw(s32 chan, s32 on) {
    static s8 last[RAPHNET_MAX_CHAN] = { -1, -1 };
    static u8 armed[RAPHNET_MAX_CHAN];
    /* a failed motor write may still have landed: until one succeeds, an off still goes out */
    static u8 unsure[RAPHNET_MAX_CHAN];
    static u8 fails[RAPHNET_MAX_CHAN];

    if (!raphnetIsOpen() || chan < 0 || chan >= RAPHNET_MAX_CHAN) {
        return -1;
    }
    on = on != 0;
    if (last[chan] == (s8)on) {
        return 0;
    }
    /* No Rumble Pak must cost nothing: stopping a motor never armed is a no-op,
       and a pak that did not answer the arm is left alone for RN_NOPAK_RETRY_MS. */
    if (!armed[chan] && !on && !unsure[chan]) {
        last[chan] = 0;
        return 0;
    }
    if (!armed[chan] && !(unsure[chan] && !on) && s_NoPakUntil[chan] != 0 &&
        (int)(RN_TICKS() - s_NoPakUntil[chan]) < 0) {
        return 0;
    }
    raphnetSuspendOnce();
    if (!armed[chan]) {
        s_RumbleWrites++;
        if (raphnetPakWrite(chan, 1024, 0x80) != 0) {
            if (!s_NoPakLogged[chan]) {
                s_NoPakLogged[chan] = 1;
                Port_Log("raphnet: no Rumble Pak answered on port %d; retried every %d s", (int)chan + 1,
                         RN_NOPAK_RETRY_MS / 1000);
            }
            s_NoPakUntil[chan] = RN_TICKS() + RN_NOPAK_RETRY_MS;
            return 0;
        }
        armed[chan] = 1;
    }
    s_RumbleWrites++;
    if (raphnetPakWrite(chan, 1536, on ? 0x01 : 0x00) != 0) {
        armed[chan] = 0; /* the pak went: arm it again next time */
        s_NoPakUntil[chan] = RN_TICKS() + RN_NOPAK_RETRY_MS;
        last[chan] = -1;
        unsure[chan] = 1;
        if (++fails[chan] < 3) {
            return -1; /* the poller retries after RN_RUMBLE_MIN_MS */
        }
        fails[chan] = 0;
        return 0;
    }
    last[chan] = (s8)on;
    unsure[chan] = 0;
    fails[chan] = 0;
    return 0;
}
