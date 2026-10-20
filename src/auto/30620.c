#include "common.h"
#include "m2c.h"

void Body_ClearContacts(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x350) = 0;
    M2C_FIELD(arg0, s32 *, 0x354) = 0;
}
