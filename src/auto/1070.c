#include "common.h"
#include "m2c.h"

extern u8 D_800D31C0[];
extern u8 D_80131F90[];
extern u8 D_80134280[];
extern u8 D_BD4A0[];
extern u8 D_BF790[];
/* end of the first ROM block: same address as D_BF790, a separate symbol in the original */
extern u8 gRomData1End[] __asm__("D_BF790");
extern u8 gResArchiveRom[];
void Game_Init(void *);
void Dma_LoadAndFlush(void *, u32, u32);
void Audio_LoadBanks(void);
void Game_MainLoop(void *);

void Boot_LoadData(void) {
    u32 start;

    Game_Init(D_800D31C0);
    start = (u32)D_BD4A0;
    Dma_LoadAndFlush(D_80131F90, start, (u32)gRomData1End - start);
    Audio_LoadBanks();
    start = (u32)D_BF790;
    Dma_LoadAndFlush(D_80134280, start, (u32)gResArchiveRom - start);
    Game_MainLoop(D_800D31C0);
}
