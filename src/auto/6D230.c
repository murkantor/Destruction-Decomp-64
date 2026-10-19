#include "common.h"
#include "m2c.h"

M2C_UNK osRecvMesg(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8006C730();                            /* extern */
M2C_UNK __osContGetInitData(M2C_UNK *, M2C_UNK);          /* extern */
M2C_UNK __osPackRequestData(M2C_UNK);                     /* extern */
s32 __osSiRawStartDma(M2C_UNK, M2C_UNK *);              /* extern */
M2C_UNK __osSiGetAccess();                            /* extern */
M2C_UNK __osSiRelAccess();                            /* extern */
extern M2C_UNK D_800D3180;
extern u8 D_80125C42;

s32 func_8006C630(s32 arg0, M2C_UNK arg1) {
    M2C_UNK sp10;
    s32 temp_s0;

    __osSiGetAccess();
    if (D_80125C42 != 0xFF) {
        func_8006C730();
        __osSiRawStartDma(1, &D_800D3180);
        osRecvMesg(arg0, 0, 1);
        __osSiRawStartDma(0, &D_800D3180);
        osRecvMesg(arg0, 0, 1);
        __osPackRequestData(0xFF);
        __osSiRawStartDma(1, &D_800D3180);
        osRecvMesg(arg0, 0, 1);
        D_80125C42 = 0xFF;
    }
    temp_s0 = __osSiRawStartDma(0, &D_800D3180);
    osRecvMesg(arg0, 0, 1);
    __osContGetInitData(&sp10, arg1);
    __osSiRelAccess();
    return temp_s0;
}
