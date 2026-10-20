#include "common.h"
#include "m2c.h"

M2C_UNK func_80024690(s32, void *, void *);
s32 Pad_GetLatched();

void func_800247FC(void *arg0)
{
  void *new_var2;
  void *new_var;
  s32 temp_v0;
  new_var = arg0;
  temp_v0 = Pad_GetLatched();
  new_var2 = new_var + 0x18;
  if ((*((s32 *) (((s8 *) new_var) + 0x14))) == 0)
  {
    func_80024690(temp_v0, new_var, new_var2);
  }
}
