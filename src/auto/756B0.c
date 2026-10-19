#include "common.h"
#include "m2c.h"

M2C_UNK osCreateMesgQueue(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 __osSiAccessQueueEnabled;
extern M2C_UNK siAccessBuf;
extern M2C_UNK __osSiAccessQueue;

void __osSiCreateAccessQueue(void) {
    __osSiAccessQueueEnabled = 1;
    osCreateMesgQueue(&__osSiAccessQueue, &siAccessBuf, 1);
    osSendMesg(&__osSiAccessQueue, 0, 0);
}
