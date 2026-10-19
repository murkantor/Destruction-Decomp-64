#include "common.h"
#include "m2c.h"

M2C_UNK osContStartReadData(M2C_UNK *);                   /* extern */
extern M2C_UNK gSiMesgQueue;

void func_80058B58(void) {
    osContStartReadData(&gSiMesgQueue);
}
