#include "common.h"
#include "m2c.h"

extern s32 D_800B04A0;
extern s32 D_800BCA60;

void func_80054044(s32 arg0) {
    D_800B04A0 = 0;
    D_800BCA60 = arg0;
}
