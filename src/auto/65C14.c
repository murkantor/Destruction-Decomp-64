#include "common.h"
#include "m2c.h"

void *func_80065014(void *arg0, u8 *arg1)
{
  s16 new_var2;
  int new_var;
  new_var2 = (s16) (*arg1);
  *((s16 *) (((s8 *) arg0) + 0xAA)) = 0;
  *((s16 *) (((s8 *) arg0) - (new_var = -0xAC))) = new_var2;
  return arg1 + 1;
}
