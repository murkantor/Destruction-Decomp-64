#include "common.h"
#include "m2c.h"

u32 __osSpGetStatus();

s32 osSpTaskYielded(void *arg0)
{
  s32 temp_a0;
  u32 new_var;
  u32 temp_v0;
  new_var = __osSpGetStatus();
  temp_v0 = new_var;
  temp_a0 = temp_v0 >> 8;
  temp_a0 = temp_a0 & 1;
  if (temp_v0 & 0x80)
  {
    *((s32 *) (((s8 *) arg0) + 4)) = (s32) (((*((s32 *) (((s8 *) arg0) + 4))) | temp_a0) & (~2));
  }
  return temp_a0;
}
