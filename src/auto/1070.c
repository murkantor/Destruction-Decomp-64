#include "common.h"
#include "m2c.h"

extern u8 D_800D31C0[];
extern u8 D_80131F90[];
extern u8 D_80134280[];
extern u8 D_BD4A0[];
extern u8 D_BF790[];
/* end of the first ROM block: same address as D_BF790, a separate symbol in the original */
extern u8 gRomData1End[] ASM_ALIAS("D_BF790");
extern u8 gResArchiveRom[];
void Game_Init(void *);
void Dma_LoadAndFlush(void *, u32, u32);
void Audio_LoadBanks(void);
void Game_MainLoop(void *);
#ifdef TARGET_PC
/* The second block starts with three functions; the port points them at C versions */
void Port_BlockLoaded(void *, u32);
#define PORT_BLOCK_LOADED(dst, size) Port_BlockLoaded(dst, size)
#else
#define PORT_BLOCK_LOADED(dst, size)
#endif

void Boot_LoadData(void) {
    u32 start;

    Game_Init(D_800D31C0);
    start = (u32)D_BD4A0;
    Dma_LoadAndFlush(D_80131F90, start, (u32)gRomData1End - start);
    PORT_BLOCK_LOADED(D_80131F90, (u32)gRomData1End - start);
    Audio_LoadBanks();
    start = (u32)D_BF790;
    Dma_LoadAndFlush(D_80134280, start, (u32)gResArchiveRom - start);
    PORT_BLOCK_LOADED(D_80134280, (u32)gResArchiveRom - start);
#ifdef TARGET_PC
    {
        void Port_DevBoot(void); /* port/game/devboot.c: DD64_BOOT */
        Port_DevBoot();
    }
#endif
    Game_MainLoop(D_800D31C0);
}
