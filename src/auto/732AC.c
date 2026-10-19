#include "common.h"
#include "m2c.h"

s32 alAuxBusParam(void *arg0, s32 arg1, s32 arg2)
{
  s32 new_var;
  s32 temp_v0;
  new_var = *((s32 *) (((s8 *) arg0) + 0x1C));
  if (arg1 == 2)
  {
    temp_v0 = *((s32 *) (((s8 *) arg0) + 0x14));
    *((s32 *) (((s8 *) arg0) + 0x14)) = (s32) (temp_v0 + 1);
    *((M2C_UNK *) ((temp_v0 * 4) + new_var)) = arg2;
  }
  return 0;
}
