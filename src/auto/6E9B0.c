#include "common.h"
#include "m2c.h"

M2C_UNK osRecvMesg(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8006DED4();                            /* extern */
s32 __osSiRawStartDma(M2C_UNK, M2C_UNK *);              /* extern */
M2C_UNK __osSiGetAccess();                            /* extern */
M2C_UNK __osSiRelAccess();                            /* extern */
extern M2C_UNK D_800D3180;
extern u8 D_80125C42;

s32 func_8006DDB0(s32 arg0) {
    s32 temp_v0;

    __osSiGetAccess();
    if (D_80125C42 != 1) {
        func_8006DED4();
        __osSiRawStartDma(1, &D_800D3180);
        osRecvMesg(arg0, 0, 1);
    }
    temp_v0 = __osSiRawStartDma(0, &D_800D3180);
    D_80125C42 = 1;
    __osSiRelAccess();
    return temp_v0;
}
