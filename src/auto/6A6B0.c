#include "common.h"
#include "m2c.h"

M2C_UNK guMtxIdentF();

void guScaleF(void *arg0, f32 arg1, f32 arg2, f32 arg3)
{
  f32 *new_var2;
  s8 *new_var;
  new_var = ((s8 *) arg0) + 0x3C;
 do { if (new_var) { guMtxIdentF(); *((f32 *) (((s8 *) arg0) + 0)) = arg1; *((f32 *) (((s8 *) arg0) + 0x14)) = arg2; new_var2 = (f32 *) new_var; *((f32 *) (((s8 *) arg0) + 0x28)) = arg3; *new_var2 = 1.0f; } else { guMtxIdentF(); *((f32 *) (((s8 *) arg0) + 0)) = arg1; *((f32 *) (((s8 *) arg0) + 0x14)) = arg2; new_var2 = (f32 *) new_var; *((f32 *) (((s8 *) arg0) + 0x28)) = arg3; *new_var2 = 1.0f; } } while (0);
}
