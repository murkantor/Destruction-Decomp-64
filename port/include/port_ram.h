/* The table tools/port/gen_data.py writes, applied by port/src/ram.c. stdint
 * types only: the generated file declares every game function generically. */
#ifndef PORT_RAM_H
#define PORT_RAM_H

#include <stdint.h>

enum {
    FIX_SWAP16, /* byte-swap a u16 */
    FIX_SWAP32, /* byte-swap a u32 or f32 */
    FIX_SWAP64, /* byte-swap a u64 or f64 */
    FIX_PTR,    /* N64 address of data: rewrite as a host pointer */
    FIX_FUNC    /* N64 address of a function: rewrite as the host function */
};

typedef struct {
    uint32_t off; /* from gPortRamVram */
    uint32_t kind;
    const void *func;
} PortRamFix;

typedef struct {
    uint32_t addr;
    const void *func;
} PortFuncAddr;

extern const PortRamFix gPortBlockFixes[];
extern const uint32_t gPortBlockFixCount;
extern const PortRamFix gPortRamFixes[];
extern const uint32_t gPortRamFixCount;
extern const PortFuncAddr gPortFuncAddrs[];
extern const uint32_t gPortFuncAddrCount;
extern const PortFuncAddr gPortAllFuncs[];
extern const uint32_t gPortAllFuncCount;
extern const uint32_t gPortRamVram;
extern const uint32_t gPortRamRomStart;
extern const uint32_t gPortRamRomEnd;
extern const uint32_t gPortRamEnd;

#endif
