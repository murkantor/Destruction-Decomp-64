#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);

s32 func_80052334(s32 arg0)
{
  s32 temp_s0;
  s32 var_a0;
  void *temp_a0;
  var_a0 = arg0;
  if (var_a0 != 0)
  {
    do
    {
      var_a0 = var_a0 - 8;
      temp_a0 = var_a0;
      temp_s0 = *((s32 *) (((s8 *) temp_a0) + 4));
      Mem_Free(temp_a0);
      var_a0 = temp_s0;
    }
    while (var_a0 != 0);
  }
  return 0;
}
