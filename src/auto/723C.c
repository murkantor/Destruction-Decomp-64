#include "common.h"
#include "m2c.h"

f32 Car_GetSpeed();

void Ai_BrakeOrReverse(void *arg0)
{
  void *temp_s1;
  void *temp_v1;
 do { temp_s1 = *((void **) (((s8 *) arg0) + 0x38)); } while (0);
  if (Car_GetSpeed() < 5.0f)
  {
    *((s8 *) (((s8 *) temp_s1) + 0x6C)) = 0;
    if ((*((u8 *) (((s8 *) temp_s1) + 0x6D))) != 0)
    {
      *((u8 *) (((s8 *) temp_s1) + 0x6D)) = 0U;
    }
    else
    {
      *((u8 *) (((s8 *) temp_s1) + 0x6D)) = 1U;
    }
    *((s32 *) (((s8 *) temp_s1) + 0x10)) = 0;
    return;
  }
  *((s8 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0x2C)))) + 1)) = 1;
  *((s8 *) (((s8 *) (*((void **) (((s8 *) arg0) + 0x2C)))) + 0)) = 0;
  temp_v1 = *((void **) (((s8 *) arg0) + 0x2C));
  *((u16 *) (((s8 *) temp_v1) + 4)) = (u16) (-((s32) (*((u16 *) (((s8 *) temp_v1) + 4)))));
}
