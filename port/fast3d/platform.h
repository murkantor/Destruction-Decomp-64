#ifndef FAST3D_PLATFORM_SHIM_H
#define FAST3D_PLATFORM_SHIM_H

/**
 * Minimal stand-in for PD's port/include/platform.h - only what the vendored
 * fast3d sources use (PD_BE16/PD_BE32 byte-swaps on little-endian hosts, and
 * a VERSION string). GE's port compiles assets natively little-endian, but
 * fast3d still swaps texture/vertex data it reads from N64-format sources.
 */

#include <stdint.h>

#define PD_BSWAP16(x) __builtin_bswap16(x)
#define PD_BSWAP32(x) __builtin_bswap32(x)

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define PD_BE16(x) (x)
#define PD_BE32(x) (x)
#else
#define PD_BE16(x) PD_BSWAP16(x)
#define PD_BE32(x) PD_BSWAP32(x)
#endif

#ifndef VERSION
#define VERSION "ge007-pc"
#endif

#endif
