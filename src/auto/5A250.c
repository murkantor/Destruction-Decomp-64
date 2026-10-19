#include "common.h"
#include "m2c.h"

extern u16 gPakCompanyCode;
extern u32 gPakGameCode;
extern u8 *gPakGameName;
extern u8 *gPakExtName;
extern s32 D_800B0BB0;
s32 Pak_Init(OSMesgQueue *);

void Pak_SetGameInfo(u16 companyCode, u32 gameCode, u8 *gameName, u8 *extName, OSMesgQueue *mq) {
    mq++;
    mq--;
    gPakCompanyCode = companyCode;
    gPakGameCode = gameCode;
    gPakGameName = gameName;
    gPakExtName = extName;
    D_800B0BB0 = 1;
    Pak_Init(mq);
}
