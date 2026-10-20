#include "common.h"
#include "m2c.h"

extern u8 *gGame;
extern s32 D_800964F0[];
extern s32 D_80096500[];
void ObjList_Remove(void *, s32);

void func_8003E99C(s32 i) {
    ObjList_Remove(gGame + 0x334, D_800964F0[i]);
    D_80096500[i] = D_800964F0[i];
    D_800964F0[i] = 0;
}
