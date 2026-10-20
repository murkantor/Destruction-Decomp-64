/*
 * Big-endian data, converted to host order where the game first reads it.
 *
 * Data loaded from the ROM at run time (PI DMA, inflated zip entries) arrives
 * big-endian. The bytes are marked in a shadow map, one byte per byte of the
 * RAM image (gRam). The game code is built with
 * -fsanitize=kernel-address, so GCC puts a call to one of the __asan_* hooks
 * below before each of its memory accesses, with the address and width. The
 * first 2-, 4- or 8-byte access to a marked location byte-swaps it in place and
 * clears the mark; single bytes are the same in either order and stay marked
 * until a wider access. A store over marked bytes clears them. Bulk copies
 * carry the marks along, so a block copied before it is read is converted at
 * its new place.
 *
 * Code that reads part of a word the rest of the game reads whole (an access
 * that only partly covers converted bytes, or straddles marked and unmarked
 * bytes) is reported once per call site (CLAUDE.md lists the fixes).
 *
 * Host code that reads game memory directly (the renderer, the audio mixer)
 * uses Port_RdN64 and Port_ReadN64Bytes (port_shadow.h), which read a value
 * the way it is stored, converted or not.
 */
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "port_shadow.h"

#define RAM_VRAM PORT_RAM_VRAM
#define RAM_SIZE PORT_RAM_SIZE

extern u8 gRam[];
u8 gPortShadow[RAM_SIZE + 16];

#define SH_BE 0x01

static void copyMarks(void *d, const void *s, size_t n);
void *__real_memcpy(void *d, const void *s, size_t n);
void *__real_memmove(void *d, const void *s, size_t n);
void *__real_memset(void *d, int c, size_t n);

/*
 * The shadow of n bytes at addr, or NULL outside the RAM image. Stacks are not
 * shadowed: GCC does not instrument a function's direct accesses to its own
 * locals, so marks there would go stale. Static data copied into locals is
 * converted at boot instead (Port_ConvertStaticData).
 */
static inline u8 *shadowOf(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    if (off < RAM_SIZE && n <= RAM_SIZE - off) {
        return gPortShadow + off;
    }
    return NULL;
}

/*
 * Per-thread state of the hooks, in one struct per thread: every instrumented
 * access goes through here.
 */
typedef struct {
    /* A bulk read of marked bytes, waiting for the bulk store that copies them */
    u32 pendingSrc, pendingLen, pendingDst, pendingDstLen;
    void *pendingSrcSite;
    /* Byte-by-byte copies (below) */
    int lastByte; /* shadow of the last byte loaded, or -1 */
    u32 pendingAddr;
    void *pendingSite;
} HookState;

#ifdef _WIN32
/*
 * MinGW's thread locals are emulated (__emutls_get_address: a call, TlsGetValue
 * and Get/SetLastError per use), which was a tenth of the game's CPU time. The
 * state is kept in a TLS slot instead, read straight from the thread's TEB
 * (the slot array at fs:0xE10, slots 0-63), and made on a thread's first use.
 */
#include <windows.h>

static DWORD sTlsIndex = TLS_OUT_OF_INDEXES;

static HookState *newHookState(void) {
    HookState *t;

    if (sTlsIndex == TLS_OUT_OF_INDEXES) {
        DWORD i = TlsAlloc();
        if (i >= 64) {
            Port_Fatal("shadow: no TLS slot below 64 (got %lu)", (unsigned long)i);
        }
        InterlockedCompareExchange((volatile LONG *)&sTlsIndex, (LONG)i, (LONG)TLS_OUT_OF_INDEXES);
        if (sTlsIndex != i) {
            TlsFree(i); /* another thread got there first */
        }
    }
    t = (HookState *)calloc(1, sizeof(*t));
    if (t == NULL) {
        Port_Fatal("shadow: out of memory");
    }
    t->lastByte = -1;
    TlsSetValue(sTlsIndex, t);
    return t;
}

static inline HookState *hookState(void) {
    HookState *t = NULL;

    if (sTlsIndex < 64) {
        __asm__("movl %%fs:0xE10(,%1,4), %0" : "=r"(t) : "r"(sTlsIndex));
    }
    return t != NULL ? t : newHookState();
}
#define tls (*hookState())
#else
static __thread HookState tls = { .lastByte = -1 };
#endif
#define tPendingSrc (tls.pendingSrc)
#define tPendingLen (tls.pendingLen)
#define tPendingDst (tls.pendingDst)
#define tPendingDstLen (tls.pendingDstLen)
#define tPendingSrcSite (tls.pendingSrcSite)
#define tLastByte (tls.lastByte)
#define tPendingAddr (tls.pendingAddr)
#define tPendingSite (tls.pendingSite)

#define MAX_REPORTS 4096
static void *sReported[MAX_REPORTS];
static int sNumReported;

static void report(const char *what, u32 addr, u32 n, void *site) {
    u32 a = addr & ~3u, i;
    u8 *s = shadowOf(a, 8);
    const u8 *m = (const u8 *)a;
    char mem[20], sh[20];

    for (i = 0; i < (u32)sNumReported; i++) {
        if (sReported[i] == site) {
            return;
        }
    }
    if (sNumReported < MAX_REPORTS) {
        sReported[sNumReported++] = site;
    }
    SDL_snprintf(mem, sizeof(mem), "%02X%02X%02X%02X %02X%02X%02X%02X", m[0], m[1], m[2], m[3], m[4], m[5], m[6],
                 m[7]);
    if (s != NULL) {
        SDL_snprintf(sh, sizeof(sh), "%02X%02X%02X%02X %02X%02X%02X%02X", s[0], s[1], s[2], s[3], s[4], s[5], s[6],
                     s[7]);
    } else {
        SDL_strlcpy(sh, "?", sizeof(sh));
    }
    Port_Log("shadow: %s, %u bytes at %08X, from code %p [mem %s sh %s]", what, (unsigned)n, (unsigned)addr, site,
             mem, sh);
}

void Port_MarkBE(const void *p, uint32_t len) {
    u8 *s = shadowOf((u32)p, len);

    if (s != NULL) {
        memset(s, SH_BE, len);
    }
}

void Port_MarkNative(const void *p, uint32_t len) {
    u8 *s = shadowOf((u32)p, len);

    if (s != NULL) {
        memset(s, 0, len);
    }
}

static void swapInPlace(u8 *p, u32 n) {
    u8 t;

    switch (n) {
        case 2:
            t = p[0], p[0] = p[1], p[1] = t;
            break;
        case 4:
            t = p[0], p[0] = p[3], p[3] = t;
            t = p[1], p[1] = p[2], p[2] = t;
            break;
        case 8:
            t = p[0], p[0] = p[7], p[7] = t;
            t = p[1], p[1] = p[6], p[6] = t;
            t = p[2], p[2] = p[5], p[5] = t;
            t = p[3], p[3] = p[4], p[4] = t;
            break;
    }
}

/*
 * Shadow byte values: SH_BE marks big-endian data not yet read; the high
 * nibble records the width the byte was last written or converted with
 * (1, 2, 4 or 8 bytes), 0 when unknown. A load at another width than the
 * value was stored with reads different bytes on a little-endian host than
 * on the N64, so it is reported.
 */
static inline u8 widthMark(u32 n) {
    return (u8)(n << 4);
}

static inline u32 spread(u32 n) {
    /* the shadow bytes of an n-byte unit at width n, as a u16/u32 pattern */
    return widthMark(n) * 0x01010101u;
}

static u32 sWatch = 1; /* DD64_WATCH, below */

/* DD64_BECOPIES=1: report struct copies of big-endian data (bulkPair) */
static int beCopies(void) {
    static int on = -1;

    if (on < 0) {
        on = SDL_getenv("DD64_BECOPIES") != NULL;
    }
    return on;
}
static void watchStore(u32 addr, u32 n, void *site);

/* A typed load of n bytes touching big-endian data. */
static void convert(u32 addr, u8 *s, u32 n, void *site) {
    u32 be = 0, i;

    if (sWatch != 0) {
        watchStore(addr, n, site); /* conversions are logged with the stores */
    }
    for (i = 0; i < n; i++) {
        be += s[i] & SH_BE;
    }
    if (be == n) {
        swapInPlace((u8 *)addr, n);
    } else {
        report("load of a partly converted value", addr, n, site);
    }
    memset(s, widthMark(n), n);
}

static inline u32 shadowBits(const u8 *s, u32 n) {
    switch (n) {
        case 1:
            return s[0];
        case 2:
            return *(const u16 *)s;
        case 4:
            return *(const u32 *)s;
        default: {
            u32 v = 0, i;
            for (i = 0; i < n; i++) {
                v |= s[i];
            }
            return v;
        }
    }
}

/*
 * A load at another width than stored only reads different bytes on PC if the
 * bytes of the units involved differ: a block filled with one byte value (zeroed,
 * typically) reads the same either way.
 */
static void reportWidth(const char *what, u32 addr, u32 n, void *site) {
    u32 unit = n, i, lo, hi;
    u8 *s = shadowOf(addr, n);

    if (s == NULL) {
        return;
    }
    for (i = 0; i < n; i++) {
        u32 w = (s[i] >> 4) & 0xF;
        if (w > unit) {
            unit = w;
        }
    }
    lo = addr & ~(unit - 1);
    hi = lo + unit;
    if (hi < addr + n) {
        hi = addr + n;
    }
    if (shadowOf(lo, hi - lo) != NULL) {
        for (i = lo + 1; i < hi; i++) {
            if (*(const u8 *)i != *(const u8 *)lo) {
                report(what, addr, n, site);
                return;
            }
        }
    }
    /* All the bytes are equal (zero-filled, typically), so they read the same
       at any width: the value takes this load's width from now on. GCC drops
       the store check after a load check of the same address (x += y), so the
       store that follows would not record it. */
    if (n > 1) {
        memset(s, widthMark(n), n);
    }
}

/*
 * Byte-by-byte copies: a byte load followed by a byte store copies the byte's
 * shadow too, and a byte load of a wider value is only reported if the next
 * access is not the store of that copy. (State: tLastByte, tPendingAddr and
 * tPendingSite in tls above.)
 */
static inline void flushPendingByte(void) {
    if (tPendingSite != NULL) {
        reportWidth("byte load of a wider value", tPendingAddr, 1, tPendingSite);
        tPendingSite = NULL;
    }
}

/*
 * A bulk read of big-endian data with no store paired with it: a struct copied
 * into a local or passed by value, where GCC does not instrument the store, so
 * the copy is never converted. Reported for PORT_CONVERT at the source.
 */
static void unpairedBulk(void) {
    if (tPendingLen != 0 && tPendingSrcSite != NULL) {
        report("copy of big-endian data into a local or argument", tPendingSrc, tPendingLen, tPendingSrcSite);
    }
    tPendingLen = tPendingDstLen = 0;
    tPendingSrcSite = NULL;
}

static inline void load(u32 addr, u32 n, void *site) {
    u8 *s;
    u32 bits;

    if (tPendingLen != 0) {
        unpairedBulk();
    }
    tPendingLen = tPendingDstLen = 0; /* a bulk copy's two checks are adjacent */
    flushPendingByte();
    s = shadowOf(addr, n);
    if (s == NULL) {
        tLastByte = -1;
        return;
    }
    if (n == 1) {
        tLastByte = *s;
        if ((*s & 0xF0) && (*s & 0xF0) != widthMark(1)) {
            tPendingAddr = addr;
            tPendingSite = site;
        }
        return;
    }
    tLastByte = -1;
    if (n == 8) {
        u32 a = *(const u32 *)s, b = *(const u32 *)(s + 4);
        bits = a | b;
        if (bits & (SH_BE * 0x01010101u)) {
            convert(addr, s, n, site);
        } else if (bits != 0 && (a != spread(8) || b != spread(8))) {
            reportWidth("load at another width than stored", addr, n, site);
        }
        return;
    }
    bits = shadowBits(s, n);
    if (bits & (SH_BE * 0x01010101u)) {
        convert(addr, s, n, site);
    } else if (bits != 0 && bits != (n == 2 ? (spread(2) & 0xFFFF) : spread(4))) {
        reportWidth("load at another width than stored", addr, n, site);
    }
}

/* DD64_WATCH=hexaddr: log each code site that stores over that address or
   converts it (a load at a width that swaps it in place) */

static void watchStore(u32 addr, u32 n, void *site) {
    if (sWatch == 1) {
        const char *e = SDL_getenv("DD64_WATCH");
        sWatch = e != NULL ? (u32)SDL_strtoul(e, NULL, 16) : 0;
    }
    if (sWatch != 0 && sWatch - addr < n) {
        static void *seen[64];
        static int nseen;
        int i;
        for (i = 0; i < nseen; i++) {
            if (seen[i] == site) {
                return;
            }
        }
        if (nseen < 64) {
            seen[nseen++] = site;
        }
        Port_Log("watch: %u-byte store at %08X from code %p", (unsigned)n, (unsigned)addr, site);
    }
}

static inline void store(u32 addr, u32 n) {
    u8 *s;

    if (sWatch != 0) {
        watchStore(addr, n, __builtin_return_address(0));
    }
    if (tPendingLen != 0) {
        unpairedBulk();
    }
    tPendingLen = tPendingDstLen = 0; /* a bulk copy's two checks are adjacent */
    if (n == 1) {
        tPendingSite = NULL; /* the byte just loaded is being copied */
    } else {
        flushPendingByte();
    }
    s = shadowOf(addr, n);
    if (s == NULL) {
        tLastByte = -1;
        return;
    }
    if (n == 1) {
        /* A byte copied keeps its source's shadow; a byte stored into
           big-endian data keeps its word's big-endian layout. */
        if (tLastByte >= 0) {
            *s = (u8)tLastByte;
        } else if (!(*s & SH_BE)) {
            *s = widthMark(1);
        }
        tLastByte = -1;
        return;
    }
    tLastByte = -1;
    memset(s, widthMark(n), n);
}

/* Struct copies: GCC checks the whole destination and source before an inline
   copy, in either order. A source with marks hands them to the destination. */
static void bulkPair(void) {
    if (tPendingLen != 0 && tPendingLen == tPendingDstLen) {
        if (tPendingSrcSite != NULL && shadowOf(tPendingDst, tPendingLen) == NULL) {
            /* big-endian data copied off the RAM image (onto a stack): the marks are lost */
            report("copy of big-endian data into a local or argument", tPendingSrc, tPendingLen,
                   tPendingSrcSite);
        } else if (tPendingSrcSite != NULL && beCopies()) {
            /* DD64_BECOPIES=1: every struct copy of unconverted data, for review: the
               marks follow the copy, but GCC may also use the values it copied
               through registers (Track_LoadProps' prop positions), which no hook
               sees */
            report("struct copy of big-endian data", tPendingSrc, tPendingLen, tPendingSrcSite);
        }
        copyMarks((void *)tPendingDst, (const void *)tPendingSrc, tPendingLen);
        tPendingLen = tPendingDstLen = 0;
        tPendingSrcSite = NULL;
    }
}

static inline void loadN(u32 addr, u32 n, void *site) {
    u8 *s;
    u32 i, be = 0;

    tLastByte = -1;
    if (tPendingLen != 0) {
        unpairedBulk();
    }
    if (n == 0 || (s = shadowOf(addr, n)) == NULL || !shadowBits(s, n)) {
        tPendingLen = 0;
        return;
    }
    for (i = 0; i < n && !be; i++) {
        /* a zero byte reads the same at any width: padding after a struct's u8 fields */
        be = (s[i] & SH_BE) && *(const u8 *)(addr + i) != 0;
    }
    tPendingSrc = addr;
    tPendingLen = n;
    tPendingSrcSite = be ? site : NULL; /* only big-endian sources matter if unpaired */
    bulkPair();
}

static inline void storeN(u32 addr, u32 n) {
    u8 *s;

    tLastByte = -1;
    if (tPendingLen != 0 && tPendingLen != n) {
        unpairedBulk();
    }
    if (n != 0 && (s = shadowOf(addr, n)) != NULL) {
        memset(s, 0, n);
    }
    tPendingDst = addr;
    tPendingDstLen = n;
    bulkPair();
}

#define SITE __builtin_return_address(0)

void __asan_load1_noabort(u32 a) { load(a, 1, SITE); }
void __asan_load2_noabort(u32 a) { load(a, 2, SITE); }
void __asan_load4_noabort(u32 a) { load(a, 4, SITE); }
void __asan_load8_noabort(u32 a) { load(a, 8, SITE); }
void __asan_load16_noabort(u32 a) { loadN(a, 16, SITE); }
void __asan_loadN_noabort(u32 a, u32 n) { loadN(a, n, SITE); }
void __asan_store1_noabort(u32 a) { store(a, 1); }
void __asan_store2_noabort(u32 a) { store(a, 2); }
void __asan_store4_noabort(u32 a) { store(a, 4); }
void __asan_store8_noabort(u32 a) { store(a, 8); }
void __asan_store16_noabort(u32 a) { storeN(a, 16); }
void __asan_storeN_noabort(u32 a, u32 n) { storeN(a, n); }
void __asan_handle_no_return(void) {}
/* Global variables the game code defines report themselves; nothing to do */
void __asan_register_globals(void *globals, size_t n) {}
void __asan_unregister_globals(void *globals, size_t n) {}

/* Block copies from game code (and anything else linked in) carry the marks. */

/* Copy the marks of n bytes from s to d, as far as d is shadowed. */
static void copyMarks(void *d, const void *s, size_t n) {
    u8 *ds = shadowOf((u32)d, n), *ss;

    if (ds == NULL || n == 0) {
        return;
    }
    ss = shadowOf((u32)s, n);
    if (ss != NULL) {
        __real_memmove(ds, ss, n);
    } else {
        __real_memset(ds, 0, n);
    }
}

/* A block copy of big-endian data to memory without a shadow loses the marks */
static void checkBlockCopy(void *d, const void *s, size_t n, void *site) {
    u8 *ss;
    size_t i;

    if (n == 0 || shadowOf((u32)d, n) != NULL || (ss = shadowOf((u32)s, n)) == NULL) {
        return;
    }
    for (i = 0; i < n; i++) {
        if (ss[i] & SH_BE) {
            report("block copy of big-endian data into a local", (u32)s, n, site);
            return;
        }
    }
}

void *__wrap_memcpy(void *d, const void *s, size_t n) {
    checkBlockCopy(d, s, n, __builtin_return_address(0));
    copyMarks(d, s, n);
    return __real_memcpy(d, s, n);
}

void *__wrap_memmove(void *d, const void *s, size_t n) {
    checkBlockCopy(d, s, n, __builtin_return_address(0));
    copyMarks(d, s, n);
    return __real_memmove(d, s, n);
}

void *__wrap_memset(void *d, int c, size_t n) {
    Port_MarkNative(d, n);
    return __real_memset(d, c, n);
}

/*
 * PORT_CONVERT (ultra64.h): game code converting count values of width bytes at
 * p before copying them where conversion on read cannot follow (a local, an
 * argument by value). Values already in host order are left alone.
 */
void Port_Convert(void *p, uint32_t count, uint32_t width) {
    u32 addr = (u32)p, k, j, be;
    u8 *s;

    for (k = 0; k < count; k++, addr += width) {
        if ((s = shadowOf(addr, width)) == NULL) {
            continue;
        }
        for (j = be = 0; j < width; j++) {
            be += s[j] & SH_BE;
        }
        if (be == width) {
            swapInPlace((u8 *)addr, width);
            memset(s, widthMark(width), width);
        }
    }
}

/*
 * Static data the game's C names, converted now by its declared type
 * (tools/port/gen_widths.py): the game may copy it into locals before reading
 * it, where the conversion on read cannot follow. Only values still wholly
 * big-endian in [lo, hi) are converted.
 */
extern const uint32_t gPortStaticWidths[][2];
extern const uint32_t gPortStaticWidthCount;

void Port_ConvertStaticData(const void *lo, uint32_t len) {
    u32 start = (u32)lo, end = start + len, i, k, done = 0;

    for (i = 0; i < gPortStaticWidthCount; i++) {
        u32 addr = gPortStaticWidths[i][0];
        u32 width = gPortStaticWidths[i][1] >> 24, count = gPortStaticWidths[i][1] & 0xFFFFFF;
        for (k = 0; k < count; k++, addr += width) {
            u8 *s;
            if (addr < start || addr + width > end || (s = shadowOf(addr, width)) == NULL) {
                continue;
            }
            u32 j, be = 0;
            for (j = 0; j < width; j++) {
                be += s[j] & SH_BE;
            }
            if (be == width) {
                swapInPlace((u8 *)addr, width);
                memset(s, widthMark(width), width);
                done++;
            }
        }
    }
    Port_Log("shadow: %u static values converted at %08X-%08X", (unsigned)done, (unsigned)start, (unsigned)end);
}

/* ---- Readers for host code ---- */

/*
 * Game memory as the N64 would hold it, for data leaving the game (Controller
 * Pak notes). A byte still big-endian, or stored as a byte, is where it is; a
 * byte of a value stored n bytes wide in host order is mirrored within its
 * n-byte unit (game data is naturally aligned). Returns 0 if some bytes are not
 * shadowed (no widths are known there), and copies those as they are.
 */
int Port_ReadN64Bytes(void *dst, const void *src, uint32_t n) {
    u8 *out = dst;
    u32 addr = (u32)src, i;
    int known = 1;

    for (i = 0; i < n; i++, addr++) {
        u8 *s = shadowOf(addr, 1);
        u32 w = s != NULL ? (*s >> 4) & 0xF : 0;
        if (s == NULL) {
            known = 0;
        }
        if (s == NULL || (*s & SH_BE) || w < 2) {
            out[i] = *(const u8 *)addr;
        } else {
            u32 base = addr & ~(w - 1);
            out[i] = *(const u8 *)(base + (w - 1) - (addr - base));
        }
    }
    return known;
}

/* Bytes in N64 order into game memory, converted where the game reads them.
   Returns 0 if the destination is not shadowed, where nothing would convert them. */
int Port_WriteN64Bytes(void *dst, const void *src, uint32_t n) {
    __real_memmove(dst, src, n);
    if (shadowOf((u32)dst, n) == NULL) {
        return 0;
    }
    Port_MarkBE(dst, n);
    return 1;
}
