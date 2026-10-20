/* The table tools/port/gen_data.py writes, applied by port/src/ram.c. */
#ifndef PORT_RAM_H
#define PORT_RAM_H

#include "ultra64.h"

enum {
    FIX_SWAP16, /* byte-swap a u16 */
    FIX_SWAP32, /* byte-swap a u32 or f32 */
    FIX_SWAP64, /* byte-swap a u64 or f64 */
    FIX_PTR,    /* N64 address of data: rewrite as a host pointer */
    FIX_FUNC    /* N64 address of a function: rewrite as the host function */
};

typedef struct {
    u32 off; /* from gPortRamVram */
    u32 kind;
    const void *func;
} PortRamFix;

typedef struct {
    u32 addr;
    const void *func;
} PortFuncAddr;

extern const PortRamFix gPortBlockFixes[];
extern const u32 gPortBlockFixCount;
extern const PortRamFix gPortRamFixes[];
extern const u32 gPortRamFixCount;
extern const PortFuncAddr gPortFuncAddrs[];
extern const u32 gPortFuncAddrCount;
extern const u32 gPortRamVram;
extern const u32 gPortRamRomStart;
extern const u32 gPortRamRomEnd;
extern const u32 gPortRamEnd;

#endif
