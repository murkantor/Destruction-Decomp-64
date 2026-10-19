#include "common.h"
#include "m2c.h"

M2C_UNK guMtxIdentF(void *);

void func_8005B220(void *arg0, s32 arg1)
{
  int new_var;
  *((s32 *) (((s8 *) arg0) + 0)) = 0;
  *((s32 *) (((s8 *) arg0) + 4)) = 0;
  *((s32 *) (((s8 *) arg0) + 0x1C)) = arg1;
  *((f32 *) (((s8 *) arg0) + 8)) = 8.0f;
  *((f32 *) (((s8 *) arg0) + 0xC)) = 4096.0f;
  *((f32 *) (((s8 *) arg0) + 0x10)) = 50.0f;
  *((f32 *) (((s8 *) arg0) + 0x14)) = 1.3333334f;
  *((f32 *) (((s8 *) arg0) + 0x18)) = 1.0f;
  switch (arg1)
  {
    case 1:
      guMtxIdentF(arg0 + 0x20);
      return;

    case 2:
      *((s8 *) (((s8 *) arg0) + 0x28)) = 1;
      *((s32 *) (((s8 *) arg0) + 0x20)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x24)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x2C)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x30)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x34)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x38)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x3C)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x40)) = 0;
      *((f32 *) (((s8 *) arg0) + 0x44)) = 1.0f;
      new_var = 0xFF;
      *((s8 *) (((s8 *) arg0) + 0x29)) = new_var;
      *((s8 *) (((s8 *) arg0) + 0x2A)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x48)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x4C)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x50)) = 0;
      *((s32 *) (((s8 *) arg0) + 0x5C)) = 0;
      *((f32 *) (((s8 *) arg0) + 0x60)) = 1.0f;
      *((s32 *) (((s8 *) arg0) + 0x64)) = 0;
      return;

  }

}
