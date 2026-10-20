/*
 * Big-endian marks on game memory: see port/src/shadow.c. Uses stdint types
 * only, so the renderer (which includes the SDK's gbi.h) can include it.
 */
#ifndef PORT_SHADOW_H
#define PORT_SHADOW_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_RAM_VRAM 0x8007ADD0u
/* RAM up to PORT_RAM_END, then 64 KB of scratch for PORT_DMA_BUF. The N64's 4 MB ends
   at 0x80400000; the port has 8 MB, as with an Expansion Pak, for the game's heap
   (the cars of a doubled field) */
#define PORT_RAM_END 0x80800000u
#define PORT_SCRATCH_SIZE 0x10000u
#define PORT_RAM_SIZE (PORT_RAM_END + PORT_SCRATCH_SIZE - PORT_RAM_VRAM)

extern unsigned char gRam[];
extern unsigned char gPortShadow[];

void Port_MarkBE(const void *p, uint32_t len);
void Port_MarkNative(const void *p, uint32_t len);
void Port_ConvertStaticData(const void *lo, uint32_t len);
int Port_ReadN64Bytes(void *dst, const void *src, uint32_t n);
int Port_WriteN64Bytes(void *dst, const void *src, uint32_t n);
void *Port_N64ToHost(uint32_t addr); /* also in port.h; the C++ renderer includes only this header */
uint32_t Port_HostToN64(const void *p);

static inline int Port_IsBEInline(const void *p) {
    uint32_t off = (uint32_t)(uintptr_t)p - (uint32_t)(uintptr_t)gRam;
    return off < PORT_RAM_SIZE && (gPortShadow[off] & 1); /* SH_BE */
}

/*
 * A 16- or 32-bit value of game memory as the N64 would read it, for host code
 * reading what the game built (display lists, vertices, matrices): big-endian
 * data is swapped; host-order data written at this width is read as it is; data
 * written at another width (a matrix built as 32-bit words read in halves, say)
 * is put back in N64 byte order first (Port_ReadN64Bytes).
 */
static inline uint32_t Port_RdN64(const void *p, uint32_t n) {
    uint32_t off = (uint32_t)(uintptr_t)p - (uint32_t)(uintptr_t)gRam, v = 0, i, same = 1, be = 1;
    uint8_t b[4];

    if (off >= PORT_RAM_SIZE) {
        memcpy(&v, p, n);
        return v;
    }
    /* the usual cases at one compare: the bytes all unmarked, all written at this
       width (n << 4), or all big-endian (1) */
    if (n == 4) {
        uint32_t s;
        memcpy(&s, &gPortShadow[off], 4);
        if (s == 0 || s == 0x40404040u) {
            memcpy(&v, p, 4);
            return v;
        }
        if (s == 0x01010101u) {
            memcpy(&v, p, 4);
            return __builtin_bswap32(v);
        }
    } else if (n == 2) {
        uint16_t s;
        memcpy(&s, &gPortShadow[off], 2);
        if (s == 0 || s == 0x2020u) {
            memcpy(&v, p, 2);
            return v;
        }
        if (s == 0x0101u) {
            memcpy(&v, p, 2);
            return (uint16_t)__builtin_bswap16((uint16_t)v);
        }
    }
    for (i = 0; i < n; i++) {
        uint8_t s = gPortShadow[off + i];
        if ((s & 1) || ((s >> 4) != 0 && (s >> 4) != n)) {
            same = 0;
        }
        if (!(s & 1)) {
            be = 0;
        }
    }
    if (same || be) {
        memcpy(&v, p, n);
        return !be ? v : n == 4 ? __builtin_bswap32(v) : (uint16_t)__builtin_bswap16((uint16_t)v);
    }
    Port_ReadN64Bytes(b, p, n);
    for (i = 0; i < n; i++) {
        v = v << 8 | b[i];
    }
    return v;
}

#ifdef __cplusplus
}
#endif

#endif
