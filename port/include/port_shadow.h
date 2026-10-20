/*
 * Big-endian marks on game memory: see port/src/shadow.c. Uses stdint types
 * only, so the renderer (which includes the SDK's gbi.h) can include it.
 */
#ifndef PORT_SHADOW_H
#define PORT_SHADOW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_RAM_VRAM 0x8007ADD0u
/* N64 RAM up to 0x80400000, then 64 KB of scratch for PORT_DMA_BUF */
#define PORT_SCRATCH_SIZE 0x10000u
#define PORT_RAM_SIZE (0x80400000u + PORT_SCRATCH_SIZE - PORT_RAM_VRAM)

extern unsigned char gRam[];
extern unsigned char gPortShadow[];

void Port_MarkBE(const void *p, uint32_t len);
void Port_MarkNative(const void *p, uint32_t len);
int Port_IsBE(const void *p);
uint16_t Port_Rd16(const void *p);
uint32_t Port_Rd32(const void *p);
void *Port_N64ToHost(uint32_t addr);
uint32_t Port_HostToN64(const void *p);

static inline int Port_IsBEInline(const void *p) {
    uint32_t off = (uint32_t)(uintptr_t)p - (uint32_t)(uintptr_t)gRam;
    return off < PORT_RAM_SIZE && (gPortShadow[off] & 1); /* SH_BE */
}

#ifdef __cplusplus
}
#endif

#endif
