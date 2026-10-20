#include "common.h"
#include "m2c.h"

M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK __osPiAccessQueue;

void __osPiRelAccess(void) {
    osSendMesg(&__osPiAccessQueue, 0, 0);
}
