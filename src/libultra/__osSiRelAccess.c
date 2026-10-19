#include "common.h"
#include "m2c.h"

M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK D_80131F10;

void __osSiRelAccess(void) {
    osSendMesg(&D_80131F10, 0, 0);
}
