#include "common.h"
#include "m2c.h"

s32 osPfsFindFile(void *, u16, s32, s32, s32, s32); /* extern */
extern M2C_UNK gPakReady;
extern u16 gPakCompanyCode;
extern s32 gPakGameCode;
extern s32 gPakGameName;
extern s32 gPakExtName;
extern M2C_UNK gPakFs;

s32 Pak_FindFile(s32 arg0, s32 arg1) {
    if (!(*((__typeof__(gPakReady) *)((s8 *)&gPakReady + (arg0 * 4))) == 0)) {
        return osPfsFindFile((arg0 * 0x68) + (s8 *)&gPakFs, gPakCompanyCode, gPakGameCode, gPakGameName, gPakExtName, arg1);
    }
    return -1;
}

s32 osPfsAllocateFile(void *, u16, s32, s32, s32, s32, s32); /* extern */

s32 Pak_AllocateFile(s32 arg0, s32 arg1, s32 arg2) {
    if (!(*((__typeof__(gPakReady) *)((s8 *)&gPakReady + (arg0 * 4))) == 0)) {
        return osPfsAllocateFile((arg0 * 0x68) + (s8 *)&gPakFs, gPakCompanyCode, gPakGameCode, gPakGameName, gPakExtName, arg1, arg2);
    }
    return -1;
}

s32 osPfsDeleteFile(void *, u16, s32, s32, s32);      /* extern */

s32 Pak_DeleteFile(s32 arg0) {
    if (!(*((__typeof__(gPakReady) *)((s8 *)&gPakReady + (arg0 * 4))) == 0)) {
        return osPfsDeleteFile((arg0 * 0x68) + (s8 *)&gPakFs, gPakCompanyCode, gPakGameCode, gPakGameName, gPakExtName);
    }
    return -1;
}
