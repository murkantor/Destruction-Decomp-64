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
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#include "port.h"
#include "port_ram.h"
#include "port_shadow.h"

#define RAM_VRAM 0x8007ADD0
#define RAM_END 0x80400000
#define RAM_SIZE (RAM_END - RAM_VRAM)
#define RAM_SKEW (RAM_VRAM & 0xFFF)

/* A no-access page follows the image, so a write past 0x80400000 faults where it happens */
#define GUARD_OFFSET ((RAM_SKEW + RAM_SIZE + 0xFFF) & ~0xFFF)
u8 gRamStorage[GUARD_OFFSET + 0x1000] __attribute__((aligned(4096)));
extern u8 gRam[]; /* gRamStorage + RAM_SKEW, defined in dd64_syms.ld */

static void protectGuard(void) {
#ifdef _WIN32
    DWORD old;
    VirtualProtect(gRamStorage + GUARD_OFFSET, 0x1000, PAGE_NOACCESS, &old);
#else
    mprotect(gRamStorage + GUARD_OFFSET, 0x1000, PROT_NONE);
#endif
}

void *Port_N64ToHost(uint32_t addr) {
    if (addr >= RAM_VRAM && addr < RAM_END) {
        return gRam + (addr - RAM_VRAM);
    }
    return NULL;
}

uint32_t Port_HostToN64(const void *p) {
    const u8 *b = p;

    if (b >= gRam && b < gRam + RAM_SIZE) {
        return RAM_VRAM + (u32)(b - gRam);
    }
    return 0;
}

void *Port_FuncFromN64(u32 addr) {
    u32 lo = 0, hi = gPortAllFuncCount;

    while (lo < hi) {
        u32 mid = (lo + hi) / 2;
        if (gPortAllFuncs[mid].addr < addr) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo < gPortAllFuncCount && gPortAllFuncs[lo].addr == addr) {
        return (void *)gPortAllFuncs[lo].func;
    }
    return NULL;
}

/*
 * N64 function pointers turn up in data the game loads at run time, where no
 * table can find them. So the N64 code addresses are made callable: a page
 * range at 0x80000000 holds, at each function's N64 address, a jump to the
 * host function. This needs those addresses free, which they are in a 32-bit
 * process on a 64-bit OS (Windows only with the large-address-aware flag).
 */
#define CODE_STUB_BASE 0x80000000u
#define CODE_STUB_SIZE 0x80000u /* through the end of the code at 0x8007ADD0 */

static void trapUnknownFunction(void) {
    Port_Fatal("The game called an N64 function the PC port does not have");
}

void Port_CodeStubsInit(void) {
    u8 *base;
    u32 i, a;

#ifdef _WIN32
    base = VirtualAlloc((void *)CODE_STUB_BASE, CODE_STUB_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
    base = mmap((void *)CODE_STUB_BASE, CODE_STUB_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (base == MAP_FAILED) {
        base = NULL;
    }
#endif
    if (base != (u8 *)CODE_STUB_BASE) {
        Port_Log("Could not map the N64 code addresses; N64 function pointers in loaded data will crash");
        return;
    }
    /* Anything else in the range traps: int3 */
    memset(base, 0xCC, CODE_STUB_SIZE);
    for (i = 0; i < gPortAllFuncCount; i++) {
        const void *target = gPortAllFuncs[i].func;
        u8 *p;
        s32 rel;

        a = gPortAllFuncs[i].addr;
        if (a < CODE_STUB_BASE || a + 5 > CODE_STUB_BASE + CODE_STUB_SIZE) {
            continue;
        }
        if (target == NULL) {
            target = (const void *)trapUnknownFunction;
        }
        p = base + (a - CODE_STUB_BASE);
        rel = (s32)((u32)target - (a + 5));
        p[0] = 0xE9; /* jmp rel32 */
        memcpy(p + 1, &rel, 4);
    }
#ifdef _WIN32
    {
        DWORD old;
        VirtualProtect(base, CODE_STUB_SIZE, PAGE_EXECUTE_READ, &old);
    }
#else
    mprotect(base, CODE_STUB_SIZE, PROT_READ | PROT_EXEC);
#endif
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
    memset(gRamStorage, 0, GUARD_OFFSET);
    protectGuard();
    memcpy(gRam, gPortRom + gPortRamRomStart, gPortRamRomEnd - gPortRamRomStart);

    /*
     * The data stays big-endian and is converted where the game first reads
     * it, at the width it reads it with (shadow.c): splat's types are guesses
     * where no code it analysed touched the data, and a struct of a short and
     * two bytes typed as a .word would be swapped wrongly. Only the pointers,
     * which splat knows from relocations, are converted now.
     */
    Port_MarkBE(gRam, gPortRamRomEnd - gPortRamRomStart);
    for (i = 0; i < gPortRamFixCount; i++) {
        const PortRamFix *f = &gPortRamFixes[i];
        if (f->kind == FIX_PTR || f->kind == FIX_FUNC) {
            fixPointer(f, gRam + f->off);
            Port_MarkNative(gRam + f->off, 4);
        }
    }
}
