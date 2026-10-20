/*
 * The N64 RAM image, mapped at its own N64 addresses.
 *
 * 0x80000000-0x80410000 is mapped at start-up (free in a 32-bit process on a
 * 64-bit OS; on Windows the executable is large-address-aware):
 *
 *   0x80000400-0x8007ADD0  the N64 code addresses: at each function's address a
 *                          jump to the host function, so N64 function pointers
 *                          in data the game loads are callable
 *   0x8007ADD0-0x80400000  rsp_text, data, rodata, bss, the two boot blocks and
 *                          the heap, where the N64 has them
 *   0x80400000-0x80410000  scratch for PORT_DMA_BUF (ultra64.h)
 *   then a no-access guard page, so a write past the end faults where it happens
 *
 * So every N64 pointer, in the data, in the boot blocks or in anything the game
 * loads from ROM, is a valid pointer as it stands. The data and rodata are
 * copied from the user's ROM; the globals the game's C names are converted to
 * host order at once, the rest when the game reads it (shadow.c).
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

#define MAP_BASE 0x80000000u
#define RAM_VRAM PORT_RAM_VRAM
#define RAM_END 0x80400000u
#define MAP_END (RAM_END + PORT_SCRATCH_SIZE)
#define CODE_END 0x8007ADD0u
#define PAGE 0x1000u

extern u8 gRam[]; /* 0x8007ADD0, from dd64_syms.ld */

void *Port_N64ToHost(uint32_t addr) {
    addr = 0x80000000u | (addr & 0x1FFFFFFF); /* KSEG1 is the same memory */
    return addr >= RAM_VRAM && addr < MAP_END ? (void *)addr : NULL;
}

uint32_t Port_HostToN64(const void *p) {
    u32 a = (u32)p;

    return a >= RAM_VRAM && a < MAP_END ? a : 0;
}

static void protect(u32 start, u32 end, int exec, int write) {
#ifdef _WIN32
    DWORD old, prot = !write ? (exec ? PAGE_EXECUTE_READ : PAGE_NOACCESS)
                             : (exec ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE);
    VirtualProtect((void *)start, end - start, prot, &old);
#else
    int prot = (write || exec) ? PROT_READ : PROT_NONE;
    if (write) prot |= PROT_WRITE;
    if (exec) prot |= PROT_EXEC;
    mprotect((void *)start, end - start, prot);
#endif
}

static void trapUnknownFunction(void) {
    Port_Fatal("The game called an N64 function the PC port does not have");
}

static void writeJump(u32 at, const void *target) {
    u8 *p = (u8 *)at;
    s32 rel = (s32)((u32)target - (at + 5));

    p[0] = 0xE9; /* jmp rel32 */
    memcpy(p + 1, &rel, 4);
}

void Port_RamInit(void) {
    u8 *base;
    u32 i;

#ifdef _WIN32
    base = VirtualAlloc((void *)MAP_BASE, MAP_END + PAGE - MAP_BASE, MEM_RESERVE | MEM_COMMIT,
                        PAGE_EXECUTE_READWRITE);
#else
    base = mmap((void *)MAP_BASE, MAP_END + PAGE - MAP_BASE, PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (base == MAP_FAILED) {
        base = NULL;
    }
#endif
    if (base != (u8 *)MAP_BASE || gRam != (u8 *)RAM_VRAM || gPortRamVram != RAM_VRAM) {
        Port_Fatal("Could not map the game's memory at 0x80000000.\n\n"
                   "Destruction Derby 64 needs a 64-bit operating system.");
    }

    /* Code addresses: a jump to each function, int3 everywhere else */
    memset(base, 0xCC, CODE_END - MAP_BASE);
    for (i = 0; i < gPortAllFuncCount; i++) {
        const void *target = gPortAllFuncs[i].func;
        u32 a = gPortAllFuncs[i].addr;

        if (a < MAP_BASE || a + 5 > CODE_END) {
            continue;
        }
        writeJump(a, target != NULL ? target : (const void *)trapUnknownFunction);
    }

    /* The data and rodata from the ROM, big-endian until read; bss and heap zero */
    memcpy(gRam, gPortRom + gPortRamRomStart, gPortRamRomEnd - gPortRamRomStart);
    memset(gRam + (gPortRamRomEnd - gPortRamRomStart), 0, MAP_END - RAM_VRAM - (gPortRamRomEnd - gPortRamRomStart));
    Port_MarkBE(gRam, gPortRamRomEnd - gPortRamRomStart);
    Port_ConvertStaticData(gRam, gPortRamRomEnd - gPortRamRomStart);

    /* The code pages read and execute only; the page shared with rsp_text stays
       writable; the guard page nothing. */
    protect(MAP_BASE, CODE_END & ~(PAGE - 1), 1, 0);
    protect(CODE_END & ~(PAGE - 1), (CODE_END & ~(PAGE - 1)) + PAGE, 1, 1);
    protect((CODE_END & ~(PAGE - 1)) + PAGE, MAP_END, 0, 1);
    protect(MAP_END, MAP_END + PAGE, 0, 0);
}

/*
 * Boot_LoadData copied a block from ROM. The block at 0x80134280 starts with
 * three small functions, which tables and loaded data point at: each gets a
 * jump to its C version (port/game/overlay.c), on a page made executable.
 */
void Port_BlockLoaded(void *dst, u32 size) {
    u32 start = (u32)dst, i;

    Port_ConvertStaticData(dst, size);

    for (i = 0; i < gPortOverlayFuncCount; i++) {
        u32 a = gPortOverlayFuncs[i].addr;
        if (a >= start && a + 5 <= start + size) {
            u32 page = a & ~(PAGE - 1);
            protect(page, page + PAGE, 1, 1);
            writeJump(a, gPortOverlayFuncs[i].func);
            Port_MarkNative((void *)a, 5);
        }
    }
}
