#include "common.h"
#include "m2c.h"

M2C_UNK func_8006EFB0(void *, u16, s32, void *, void *);
extern volatile char D_800BD290;

void func_80059B10(s32 arg0, void *arg1)
{
  func_8006EFB0((arg0 * 0x68) + (&D_800BD290), *((u16 *) (((s8 *) arg1) + 8)), *((s32 *) (((s8 *) arg1) + 4)), arg1 + 0xE, arg1 + 0xA);
}
