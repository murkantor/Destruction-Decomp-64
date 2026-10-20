/*
 * Big-endian data, converted to host order where the game first reads it.
 *
 * Data loaded from the ROM at run time (PI DMA, inflated zip entries) arrives
 * big-endian. The bytes are marked in a shadow map, one byte per byte of the
 * RAM image (gRam). The game code is built with -fsanitize=kernel-address, so
 * GCC puts a call to one of the __asan_* hooks below before each of its memory
 * accesses, with the address and width. The first 2-, 4- or 8-byte access to a
 * marked location byte-swaps it in place and clears the mark; single bytes are
 * the same in either order and stay marked until a wider access. A store over
 * marked bytes clears them. Bulk copies carry the marks along, so a block copied
 * before it is read is converted at its new place.
 *
 * Code that reads part of a word the rest of the game reads whole (an access
 * that only partly covers converted bytes, or straddles marked and unmarked
 * bytes) is reported once per call site; such places need BE_OFF8/BE_OFF16.
 *
 * Host code that reads game memory directly (the renderer, the audio mixer)
 * uses Port_Rd16/32 and friends, which byte-swap marked data as they read it.
 */
#include <SDL3/SDL.h>
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

/* A bulk read of marked bytes, waiting for the bulk store that copies them */
static __thread u32 tPendingSrc, tPendingLen, tPendingDst, tPendingDstLen;
/* The last single-byte load read a marked byte: a byte-by-byte copy carries the mark */

#define MAX_REPORTS 4096
static void *sReported[MAX_REPORTS];
static int sNumReported;

static void report(const char *what, u32 off, u32 n, void *site) {
    int i;

    for (i = 0; i < sNumReported; i++) {
        if (sReported[i] == site) {
            return;
        }
    }
    if (sNumReported < MAX_REPORTS) {
        sReported[sNumReported++] = site;
    }
    {
        u32 a = off & ~3u;
        Port_Log("shadow: %s, %u bytes at %08X (N64 %08X), from code %p [mem %02X%02X%02X%02X %02X%02X%02X%02X sh %02X%02X%02X%02X %02X%02X%02X%02X]",
                 what, (unsigned)n, (unsigned)(u32)(gRam + off), (unsigned)(RAM_VRAM + off), site, gRam[a],
                 gRam[a + 1], gRam[a + 2], gRam[a + 3], gRam[a + 4], gRam[a + 5], gRam[a + 6], gRam[a + 7],
                 gPortShadow[a], gPortShadow[a + 1], gPortShadow[a + 2], gPortShadow[a + 3], gPortShadow[a + 4],
                 gPortShadow[a + 5], gPortShadow[a + 6], gPortShadow[a + 7]);
    }
}

void Port_MarkBE(const void *p, uint32_t len) {
    u32 off = (u32)p - (u32)gRam;

    if (off < RAM_SIZE) {
        if (len > RAM_SIZE - off) {
            len = RAM_SIZE - off;
        }
        memset(gPortShadow + off, SH_BE, len);
    }
}

void Port_MarkNative(const void *p, uint32_t len) {
    u32 off = (u32)p - (u32)gRam;

    if (off < RAM_SIZE) {
        if (len > RAM_SIZE - off) {
            len = RAM_SIZE - off;
        }
        memset(gPortShadow + off, 0, len);
    }
}

int Port_IsBE(const void *p) {
    u32 off = (u32)p - (u32)gRam;

    return off < RAM_SIZE && (gPortShadow[off] & SH_BE);
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

/* A typed load of n bytes touching big-endian data. */
static void convert(u32 off, u32 n, void *site) {
    u8 *s = gPortShadow + off;
    u32 be = 0, i;

    for (i = 0; i < n; i++) {
        be += s[i] & SH_BE;
    }
    if (be == n) {
        swapInPlace(gRam + off, n);
    } else {
        report("load of a partly converted value", off, n, site);
    }
    memset(s, widthMark(n), n);
}

static inline u32 shadowBits(u32 off, u32 n) {
    const u8 *s = gPortShadow + off;

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
static void reportWidth(const char *what, u32 off, u32 n, void *site) {
    u32 unit = n, i, lo, hi;
    u8 *s = gPortShadow + off;

    for (i = 0; i < n; i++) {
        u32 w = (s[i] >> 4) & 0xF;
        if (w > unit) {
            unit = w;
        }
    }
    lo = off & ~(unit - 1);
    hi = lo + unit;
    if (hi < off + n) {
        hi = off + n;
    }
    for (i = lo + 1; i < hi && i < RAM_SIZE; i++) {
        if (gRam[i] != gRam[lo]) {
            report(what, off, n, site);
            return;
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
 * access is not the store of that copy.
 */
static __thread int tLastByte = -1;       /* shadow of the last byte loaded, or -1 */
static __thread u32 tPendingOff;
static __thread void *tPendingSite;

static inline void flushPendingByte(void) {
    if (tPendingSite != NULL) {
        reportWidth("byte load of a wider value", tPendingOff, 1, tPendingSite);
        tPendingSite = NULL;
    }
}

static inline void load(u32 addr, u32 n, void *site) {
    u32 off = addr - (u32)gRam, bits;

    flushPendingByte();
    if (off >= RAM_SIZE) {
        tLastByte = -1;
        return;
    }
    if (n == 1) {
        u8 s = gPortShadow[off];
        tLastByte = s;
        if ((s & 0xF0) && (s & 0xF0) != widthMark(1)) {
            tPendingOff = off;
            tPendingSite = site;
        }
        return;
    }
    tLastByte = -1;
    if (n == 8) {
        bits = *(const u32 *)(gPortShadow + off) | *(const u32 *)(gPortShadow + off + 4);
        if (bits & (SH_BE * 0x01010101u)) {
            convert(off, n, site);
        } else if (bits != 0 && (*(const u32 *)(gPortShadow + off) != spread(8) ||
                                 *(const u32 *)(gPortShadow + off + 4) != spread(8))) {
            reportWidth("load at another width than stored", off, n, site);
        }
        return;
    }
    bits = shadowBits(off, n);
    if (bits & (SH_BE * 0x01010101u)) {
        convert(off, n, site);
    } else if (bits != 0 && bits != (n == 2 ? (spread(2) & 0xFFFF) : spread(4))) {
        reportWidth("load at another width than stored", off, n, site);
    }
}

/* DD64_WATCH=hexaddr: log each code site that stores over that address */
static u32 sWatch = 1;

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
    u32 off = addr - (u32)gRam;

    if (sWatch != 0) {
        watchStore(addr, n, __builtin_return_address(0));
    }
    if (n == 1) {
        tPendingSite = NULL; /* the byte just loaded is being copied */
    } else {
        flushPendingByte();
    }
    if (off >= RAM_SIZE) {
        tLastByte = -1;
        return;
    }
    if (n == 1) {
        /* A byte copied keeps its source's shadow; a byte stored into
           big-endian data keeps its word's big-endian layout. */
        u8 *s = gPortShadow + off;
        if (tLastByte >= 0) {
            *s = (u8)tLastByte;
        } else if (!(*s & SH_BE)) {
            *s = widthMark(1);
        }
        tLastByte = -1;
        return;
    }
    tLastByte = -1;
    memset(gPortShadow + off, widthMark(n), n);
}

/* Struct copies: GCC checks the whole destination and source before an inline
   copy, in either order. A source with marks hands them to the destination. */
static void bulkPair(void) {
    if (tPendingLen != 0 && tPendingLen == tPendingDstLen) {
        copyMarks((void *)tPendingDst, (const void *)tPendingSrc, tPendingLen);
        tPendingLen = tPendingDstLen = 0;
    }
}

static inline void loadN(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    tLastByte = -1;
    if (off >= RAM_SIZE || n == 0 || n > RAM_SIZE - off || !shadowBits(off, n)) {
        tPendingLen = 0;
        return;
    }
    tPendingSrc = addr;
    tPendingLen = n;
    bulkPair();
}

static inline void storeN(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    tLastByte = -1;
    if (off < RAM_SIZE && n != 0) {
        memset(gPortShadow + off, 0, n < RAM_SIZE - off ? n : RAM_SIZE - off);
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
void __asan_load16_noabort(u32 a) { loadN(a, 16); }
void __asan_loadN_noabort(u32 a, u32 n) { loadN(a, n); }
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

/* Copy the marks of n bytes from s to d, as far as either lies in gRam. */
static void copyMarks(void *d, const void *s, size_t n) {
    u32 so = (u32)s - (u32)gRam, dOff = (u32)d - (u32)gRam;

    if (dOff >= RAM_SIZE) {
        return;
    }
    if (n > RAM_SIZE - dOff) {
        n = RAM_SIZE - dOff;
    }
    if (so < RAM_SIZE && n <= RAM_SIZE - so) {
        __real_memmove(gPortShadow + dOff, gPortShadow + so, n);
    } else {
        __real_memset(gPortShadow + dOff, 0, n);
    }
}

void *__wrap_memcpy(void *d, const void *s, size_t n) {
    copyMarks(d, s, n);
    return __real_memcpy(d, s, n);
}

void *__wrap_memmove(void *d, const void *s, size_t n) {
    copyMarks(d, s, n);
    return __real_memmove(d, s, n);
}

void *__wrap_memset(void *d, int c, size_t n) {
    Port_MarkNative(d, n);
    return __real_memset(d, c, n);
}

/* ---- Readers for host code ---- */

uint16_t Port_Rd16(const void *p) {
    uint16_t v;

    memcpy(&v, p, 2);
    if (Port_IsBE(p)) {
        v = (u16)(v << 8 | v >> 8);
    }
    return v;
}

uint32_t Port_Rd32(const void *p) {
    uint32_t v;

    memcpy(&v, p, 4);
    if (Port_IsBE(p)) {
        v = __builtin_bswap32(v);
    }
    return v;
}
