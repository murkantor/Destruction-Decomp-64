/* The tables tools/port/gen_data.py writes, used by port/src/ram.c. stdint
 * types only: the generated file declares every game function generically. */
#ifndef PORT_RAM_H
#define PORT_RAM_H

#include <stdint.h>

typedef struct {
    uint32_t addr; /* N64 address */
    const void *func;
} PortFuncAddr;

/* Every game function at its N64 address (func NULL where the port has none) */
extern const PortFuncAddr gPortAllFuncs[];
extern const uint32_t gPortAllFuncCount;
/* The three functions at the start of the block loaded to 0x80134280 */
extern const PortFuncAddr gPortOverlayFuncs[];
extern const uint32_t gPortOverlayFuncCount;

extern const uint32_t gPortRamVram;
extern const uint32_t gPortRamRomStart;
extern const uint32_t gPortRamRomEnd;

#endif
