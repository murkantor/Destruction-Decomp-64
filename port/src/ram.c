/*
 * The N64 RAM image: rsp_text (0x8007ADD0) up to the end of the 4 MB heap
 * (0x80400000), at the same relative layout, so every data and bss symbol is
 * gRam plus its N64 offset (build/gen/dd64_syms.ld) and code that walks from
 * one symbol into the next still works.
 *
 * At start-up the data and rodata come from the user's ROM, then the table from
 * tools/port/gen_data.py turns them into host byte order and host pointers.
 * gRam sits 0xDD0 into a 4 KB-aligned buffer so host addresses keep the N64
 * addresses' low 12 bits, which some alignment arithmetic depends on.
 */
#include <string.h>

#include "port.h"
#include "port_ram.h"
#include "port_shadow.h"

#define RAM_VRAM 0x8007ADD0
#define RAM_END 0x80400000
#define RAM_SIZE (RAM_END - RAM_VRAM)
#define RAM_SKEW (RAM_VRAM & 0xFFF)

u8 gRamStorage[RAM_SKEW + RAM_SIZE] __attribute__((aligned(4096)));
extern u8 gRam[]; /* gRamStorage + RAM_SKEW, defined in dd64_syms.ld */

void *Port_N64ToHost(u32 addr) {
    if (addr >= RAM_VRAM && addr < RAM_END) {
        return gRam + (addr - RAM_VRAM);
    }
    return NULL;
}

u32 Port_HostToN64(const void *p) {
    const u8 *b = p;

    if (b >= gRam && b < gRam + RAM_SIZE) {
        return RAM_VRAM + (u32)(b - gRam);
    }
    return 0;
}

void *Port_FuncFromN64(u32 addr) {
    u32 lo = 0, hi = gPortFuncAddrCount;

    while (lo < hi) {
        u32 mid = (lo + hi) / 2;
        if (gPortFuncAddrs[mid].addr < addr) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo < gPortFuncAddrCount && gPortFuncAddrs[lo].addr == addr) {
        return (void *)gPortFuncAddrs[lo].func;
    }
    return NULL;
}

static u32 be32(const u8 *p) {
    return (u32)p[0] << 24 | (u32)p[1] << 16 | (u32)p[2] << 8 | p[3];
}

/* Rewrite the big-endian N64 pointer at p as a host pointer. */
static void fixPointer(const PortRamFix *f, u8 *p) {
    u32 v;

    if (f->kind == FIX_FUNC) {
        v = (u32)f->func;
    } else {
        u32 addr = be32(p);
        void *host = Port_N64ToHost(addr);
        if (host == NULL && addr != 0) {
            Port_Fatal("data pointer 0x%08X at 0x%08X is outside RAM", addr, RAM_VRAM + f->off);
        }
        v = (u32)host;
    }
    memcpy(p, &v, 4);
}

/*
 * Boot_LoadData copies two blocks from ROM to fixed addresses. Their N64
 * pointers become host pointers here; everything else in them is converted
 * where it is first read, like other loaded data.
 */
void Port_ApplyBlockFixes(void *dst, u32 size) {
    u32 start = (u32)((u8 *)dst - gRam), i;

    for (i = 0; i < gPortBlockFixCount; i++) {
        const PortRamFix *f = &gPortBlockFixes[i];
        if (f->off >= start && f->off < start + size) {
            fixPointer(f, gRam + f->off);
            Port_MarkNative(gRam + f->off, 4);
        }
    }
}

void Port_RamInit(void) {
    u32 i;

    if (gPortRamVram != RAM_VRAM || gPortRamEnd != RAM_END || (u32)gRam != (u32)gRamStorage + RAM_SKEW) {
        Port_Fatal("RAM layout does not match the generated tables");
    }
    memset(gRamStorage, 0, sizeof(gRamStorage));
    memcpy(gRam, gPortRom + gPortRamRomStart, gPortRamRomEnd - gPortRamRomStart);

    for (i = 0; i < gPortRamFixCount; i++) {
        const PortRamFix *f = &gPortRamFixes[i];
        u8 *p = gRam + f->off;
        u32 v;

        switch (f->kind) {
            case FIX_SWAP16: {
                u16 h = (u16)(p[0] << 8 | p[1]);
                memcpy(p, &h, 2);
                break;
            }
            case FIX_SWAP32:
                v = be32(p);
                memcpy(p, &v, 4);
                break;
            case FIX_SWAP64: {
                u64 d = (u64)be32(p) << 32 | be32(p + 4);
                memcpy(p, &d, 8);
                break;
            }
            case FIX_PTR:
            case FIX_FUNC:
                fixPointer(f, p);
                break;
        }
    }
}
