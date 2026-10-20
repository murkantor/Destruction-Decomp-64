#include "common.h"
#include "m2c.h"

s16 Gfx_CountCommandKind(s32, M2C_UNK, M2C_UNK);

u16 Model_Init(void *arg0, s32 arg1, u16 (*arg2)(M2C_UNK, void *, void *), int arg3)
{
  s32 temp_a0;
  u16 (*temp_v0)(M2C_UNK, void *, void *);
  *((s32 *) (((s8 *) arg0) + 0xC)) = arg1;
  ;
  *((u16 (**)(M2C_UNK, void *, void *)) (((s8 *) arg0) + 0x10)) = arg2;
  *((s32 *) (((s8 *) arg0) + 0x14)) = arg3;
  if ((*((s32 *) (((s8 *) arg0) + 0xC))) != 0)
  {
    *((s16 *) (((s8 *) arg0) + 2)) = Gfx_CountCommandKind(*((s32 *) (((s8 *) arg0) + 0xC)), 0xA, 0);
  }
  else
  {
    *((s16 *) (((s8 *) arg0) + 2)) = 0;
  }
  temp_v0 = *((u16 (**)(M2C_UNK, void *, void *)) (((s8 *) arg0) + 0x10));
  *((s32 *) (((s8 *) arg0) + 4)) = 0;
  *((s32 *) (((s8 *) arg0) + 8)) = 0;
  if (temp_v0 != 0)
  {
    *((u16 *) (((s8 *) arg0) + 0)) = temp_v0(0, arg0 + 4, arg0 + 8);
  }
  else
  {
    *((u16 *) (((s8 *) arg0) + 0)) = 0U;
  }
  return *((u16 *) (((s8 *) arg0) + 0));
}


u16 func_8005A9EC(void *arg0, s32 arg1, u16 (*arg2)(M2C_UNK, void *, void *), s32 arg3)
{
  s32 temp_a0;
  u16 (*temp_v0)(M2C_UNK, void *, void *);
  *((s32 *) (((s8 *) arg0) + 0xC)) = arg1;
  temp_a0 = *((s32 *) (((s8 *) arg0) + 0xC));
  *((u16 (**)(M2C_UNK, void *, void *)) (((s8 *) arg0) + 0x10)) = arg2;
  *((s32 *) (((s8 *) arg0) + 0x14)) = arg3;
  if ((*((s32 *) (((s8 *) arg0) + 0xC))) != 0)
  {
    *((s16 *) (((s8 *) arg0) + 2)) = Gfx_CountCommandKind(*((s32 *) (((s8 *) arg0) + 0xC)), 0xA, 0);
  }
  else
  {
    *((s16 *) (((s8 *) arg0) + 2)) = 0;
  }
  temp_v0 = *((u16 (**)(M2C_UNK, void *, void *)) (((s8 *) arg0) + 0x10));
  *((s32 *) (((s8 *) arg0) + 4)) = 0;
  *((s32 *) (((s8 *) arg0) + 8)) = 0;
  if (temp_v0 != 0)
  {
    *((u16 *) (((s8 *) arg0) + 0)) = temp_v0(0, arg0 + 4, arg0 + 8);
  }
  else
  {
    *((u16 *) (((s8 *) arg0) + 0)) = 0U;
  }
  return *((u16 *) (((s8 *) arg0) + 0));
}
