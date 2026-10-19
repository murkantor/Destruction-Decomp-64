#include "common.h"
#include "m2c.h"

void *func_80065400(void *arg0, u8 *arg1)
{
  u8 new_var;
  new_var = *arg1;
  *((s8 *) (((s8 *) arg0) + 0xCA)) = 0;
  *((u8 *) (((s8 *) arg0) + 0xCB)) = (u8) new_var;
  return arg1 + 1;
}
