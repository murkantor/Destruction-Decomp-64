#include "common.h"
#include "m2c.h"

M2C_UNK func_8006FAA0(void *);
extern unsigned char gPakFs;

void func_80059A94(s32 arg0)
{
  func_8006FAA0((arg0 * 0x68) + (&gPakFs));
}
