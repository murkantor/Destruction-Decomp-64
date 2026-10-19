#include "common.h"
#include "m2c.h"

u32 alHeapDBAlloc(M2C_UNK arg0, M2C_UNK arg1, void *arg2, s32 arg3, s32 arg4)
{
  u32 temp_a0;
  u32 temp_a1;
  int new_var;
  u32 var_t0;
  new_var = ~0xF;
  temp_a1 = *((u32 *) (((s8 *) arg2) + 4));
  temp_a0 = temp_a1 + (((arg3 * arg4) + 0xF) & new_var);
  var_t0 = 0;
  if (((u32) ((*((s32 *) (((s8 *) arg2) + 0))) + (*((s32 *) (((s8 *) arg2) + 8))))) >= temp_a0)
  {
    var_t0 = temp_a1;
    *((u32 *) (((s8 *) arg2) + 4)) = temp_a1 + (((arg3 * arg4) + 0xF) & new_var);
  }
  return var_t0;
}
