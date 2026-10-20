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
#include <string.h>

#include "port.h"
#include "port_shadow.h"

#define RAM_VRAM 0x8007ADD0
#define RAM_END 0x80400000
#define RAM_SIZE (RAM_END - RAM_VRAM)

extern u8 gRam[];
u8 gPortShadow[RAM_SIZE + 16];

/* A bulk read of marked bytes, waiting for the bulk store that copies them */
static __thread u32 tPendingSrc, tPendingLen, tPendingDst, tPendingDstLen;
/* The last single-byte load read a marked byte: a byte-by-byte copy carries the mark */
static __thread int tLastByteMarked;

#define MAX_REPORTS 256
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
    Port_Log("shadow: %s, %u bytes at %08X (N64 %08X), from code %p", what, (unsigned)n,
             (unsigned)(u32)(gRam + off), (unsigned)(RAM_VRAM + off), site);
}

void Port_MarkBE(const void *p, u32 len) {
    u32 off = (u32)p - (u32)gRam;

    if (off < RAM_SIZE) {
        if (len > RAM_SIZE - off) {
            len = RAM_SIZE - off;
        }
        memset(gPortShadow + off, 1, len);
    }
}

void Port_MarkNative(const void *p, u32 len) {
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

    return off < RAM_SIZE && gPortShadow[off];
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

/* A typed load of n bytes touching marked data. */
static void convert(u32 off, u32 n, void *site) {
    u8 *s = gPortShadow + off;
    u32 marked = 0, i;

    for (i = 0; i < n; i++) {
        marked += s[i];
    }
    if (marked == n) {
        swapInPlace(gRam + off, n);
    } else {
        report("partly converted value read", off, n, site);
    }
    memset(s, 0, n);
}

static inline int anyMarked(u32 off, u32 n) {
    const u8 *s = gPortShadow + off;

    switch (n) {
        case 1:
            return s[0];
        case 2:
            return *(const u16 *)s;
        case 4:
            return *(const u32 *)s;
        case 8:
            return *(const u32 *)s | *(const u32 *)(s + 4);
        default: {
            u32 i;
            for (i = 0; i < n; i++) {
                if (s[i]) {
                    return 1;
                }
            }
            return 0;
        }
    }
}

static inline void load(u32 addr, u32 n, void *site) {
    u32 off = addr - (u32)gRam;

    if (off >= RAM_SIZE) {
        tLastByteMarked = 0;
        return;
    }
    if (n == 1) {
        tLastByteMarked = gPortShadow[off];
        return;
    }
    tLastByteMarked = 0;
    if (anyMarked(off, n)) {
        convert(off, n, site);
    }
}

static inline void store(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    if (off >= RAM_SIZE) {
        tLastByteMarked = 0;
        return;
    }
    if (n == 1) {
        /* A marked byte keeps the big-endian layout of its word; a byte copied
           from marked data is marked too. */
        if (tLastByteMarked) {
            gPortShadow[off] = 1;
        }
        tLastByteMarked = 0;
        return;
    }
    tLastByteMarked = 0;
    if (anyMarked(off, n)) {
        memset(gPortShadow + off, 0, n);
    }
}

/* Struct copies: GCC checks the whole destination and source before an inline
   copy, in either order. A source with marks hands them to the destination. */
static void bulkPair(void) {
    u32 so = tPendingSrc - (u32)gRam, dOff = tPendingDst - (u32)gRam;

    if (tPendingLen != 0 && tPendingLen == tPendingDstLen && so < RAM_SIZE) {
        if (dOff < RAM_SIZE) {
            memmove(gPortShadow + dOff, gPortShadow + so, tPendingLen);
        }
        tPendingLen = tPendingDstLen = 0;
    }
}

static inline void loadN(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    tLastByteMarked = 0;
    if (off >= RAM_SIZE || n == 0 || !anyMarked(off, n)) {
        tPendingLen = 0;
        return;
    }
    tPendingSrc = addr;
    tPendingLen = n;
    bulkPair();
}

static inline void storeN(u32 addr, u32 n) {
    u32 off = addr - (u32)gRam;

    tLastByteMarked = 0;
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

/* Block copies from game code (and anything else linked in) carry the marks. */
void *__real_memcpy(void *d, const void *s, size_t n);
void *__real_memmove(void *d, const void *s, size_t n);
void *__real_memset(void *d, int c, size_t n);

void *__wrap_memcpy(void *d, const void *s, size_t n) {
    u32 so = (u32)s - (u32)gRam, dOff = (u32)d - (u32)gRam;

    if (dOff < RAM_SIZE) {
        if (so < RAM_SIZE) {
            __real_memmove(gPortShadow + dOff, gPortShadow + so, n);
        } else {
            __real_memset(gPortShadow + dOff, 0, n);
        }
    }
    return __real_memcpy(d, s, n);
}

void *__wrap_memmove(void *d, const void *s, size_t n) {
    u32 so = (u32)s - (u32)gRam, dOff = (u32)d - (u32)gRam;

    if (dOff < RAM_SIZE) {
        if (so < RAM_SIZE) {
            __real_memmove(gPortShadow + dOff, gPortShadow + so, n);
        } else {
            __real_memset(gPortShadow + dOff, 0, n);
        }
    }
    return __real_memmove(d, s, n);
}

void *__wrap_memset(void *d, int c, size_t n) {
    Port_MarkNative(d, n);
    return __real_memset(d, c, n);
}

/* ---- Readers for host code ---- */

u16 Port_Rd16(const void *p) {
    u16 v;

    memcpy(&v, p, 2);
    if (Port_IsBE(p)) {
        v = (u16)(v << 8 | v >> 8);
    }
    return v;
}

u32 Port_Rd32(const void *p) {
    u32 v;

    memcpy(&v, p, 4);
    if (Port_IsBE(p)) {
        v = __builtin_bswap32(v);
    }
    return v;
}
