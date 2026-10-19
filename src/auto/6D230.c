#include "common.h"
#include "m2c.h"

M2C_UNK osRecvMesg(s32, M2C_UNK, M2C_UNK);       /* extern */
M2C_UNK func_8006C730();                            /* extern */
M2C_UNK __osContGetInitData(M2C_UNK *, M2C_UNK);          /* extern */
M2C_UNK __osPackRequestData(M2C_UNK);                     /* extern */
s32 __osSiRawStartDma(M2C_UNK, M2C_UNK *);              /* extern */
M2C_UNK __osSiGetAccess();                            /* extern */
M2C_UNK __osSiRelAccess();                            /* extern */
extern M2C_UNK __osContPifRam;
extern u8 __osContLastCmd;

s32 func_8006C630(s32 arg0, M2C_UNK arg1) {
    M2C_UNK sp10;
    s32 temp_s0;

    __osSiGetAccess();
    if (__osContLastCmd != 0xFF) {
        func_8006C730();
        __osSiRawStartDma(1, &__osContPifRam);
        osRecvMesg(arg0, 0, 1);
        __osSiRawStartDma(0, &__osContPifRam);
        osRecvMesg(arg0, 0, 1);
        __osPackRequestData(0xFF);
        __osSiRawStartDma(1, &__osContPifRam);
        osRecvMesg(arg0, 0, 1);
        __osContLastCmd = 0xFF;
    }
    temp_s0 = __osSiRawStartDma(0, &__osContPifRam);
    osRecvMesg(arg0, 0, 1);
    __osContGetInitData(&sp10, arg1);
    __osSiRelAccess();
    return temp_s0;
}
