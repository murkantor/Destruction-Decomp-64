#include "common.h"
#include "m2c.h"

M2C_UNK func_8006F1D0(void *, M2C_UNK, M2C_UNK);
extern char D_800BD290;

void func_80059ACC(s32 arg0, M2C_UNK arg1, M2C_UNK arg2)
{
  func_8006F1D0((arg0 * 0x68) + (&D_800BD290), arg2, arg1);
}
