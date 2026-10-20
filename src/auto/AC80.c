#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);
M2C_UNK ModelInst_Free(s32);
extern M2C_UNK D_80125B70;

void Debris_FreeCar(void *arg0)
{
  s32 temp_s1;
  s32 var_v0;
  void *temp_a0;
  void *var_s0;
  temp_s1 = *((s32 *) (((s8 *) arg0) + 0x34));
  var_v0 = temp_s1 * 4;
  var_s0 = *((typeof(D_80125B70) *) (((s8 *) (&D_80125B70)) + var_v0));
  if (var_s0 != 0)
  {
    do
    {
      ModelInst_Free(var_s0 + 0x28);
      var_v0 = 0x50;
      temp_a0 = var_s0;
      var_s0 = *((void **) (((s8 *) var_s0) + var_v0));
      Mem_Free(temp_a0);
      var_v0 = temp_s1 * 4;
    }
    while (var_s0 != 0);
  }
  *((typeof(D_80125B70) *) (((s8 *) (&D_80125B70)) + var_v0)) = 0;
}
