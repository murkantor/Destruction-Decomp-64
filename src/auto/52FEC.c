#include "common.h"
#include "m2c.h"

extern void *D_800BCA34;
extern u32 D_800BCA38;
extern u32 D_800BCA3C;
extern u8 *D_800BCA40;
extern u32 D_800BCA44;
extern u32 D_800BCA48;
extern s32 D_800B048C;
s32 RomFile_Read(void *, u32, s32, void *);

s32 Inflate_GetByte(void) {
    if (D_800BCA44 < D_800BCA48) {
        return D_800BCA40[D_800BCA44++];
    }
    if (D_800BCA38 < D_800BCA3C) {
        D_800BCA48 = D_800BCA3C - D_800BCA38;
        if (D_800BCA48 > 0x4000) {
            D_800BCA48 = 0x4000;
        }
        if (RomFile_Read(D_800BCA40, D_800BCA48, 1, D_800BCA34) == 1) {
            D_800BCA44 = 1;
            D_800BCA38 += D_800BCA48;
            return D_800BCA40[0];
        }
    }
    D_800B048C = 1;
    return 0x100;
}
