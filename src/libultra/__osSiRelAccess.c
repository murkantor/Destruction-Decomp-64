#include "common.h"
#include "m2c.h"

M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK __osSiAccessQueue;

void __osSiRelAccess(void) {
    osSendMesg(&__osSiAccessQueue, 0, 0);
}
