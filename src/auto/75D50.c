#include "common.h"
#include "m2c.h"

M2C_UNK osCreateMesgQueue(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 __osPiAccessQueueEnabled;
extern M2C_UNK D_800D1070;
extern M2C_UNK __osPiAccessQueue;

void __osPiCreateAccessQueue(void) {
    __osPiAccessQueueEnabled = 1;
    osCreateMesgQueue(&__osPiAccessQueue, &D_800D1070, 1);
    osSendMesg(&__osPiAccessQueue, 0, 0);
}
