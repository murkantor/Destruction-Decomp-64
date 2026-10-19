#include "common.h"
#include "m2c.h"

void func_800553B4(void **arg0, s32 *arg1, s32 *arg2)
{
  s32 temp_v0;
  s32 var_v1;
  s32 var_a3;
  void *var_a0;
  var_a0 = *arg0;
  var_v1 = 0;
  var_a3 = 0;
  if (var_a0 != 0)
  {
    do
    {
      temp_v0 = *((s32 *) (((s8 *) var_a0) + 0));
      var_a0 = *((void **) (((s8 *) var_a0) + 4));
      var_a3 += 1;
      var_v1 += temp_v0;
    }
    while (var_a0 != 0);
  }
  *arg1 = var_v1;
  *arg2 = var_a3;
}
