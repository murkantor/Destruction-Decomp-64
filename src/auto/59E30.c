#include "common.h"
#include "m2c.h"

extern unsigned char gControllers;

void Pad_Clear(s32 arg0)
{
  void *temp_v0;
  s8 *new_var;
 do { temp_v0 = (arg0 * 0xB0) + (&gControllers); if (1) { *((s32 *) (((s8 *) temp_v0) + 8)) = 0; *((s32 *) (((s8 *) temp_v0) + 0xC)) = 0; new_var = (s8 *) temp_v0; *((s32 *) (new_var + 0x10)) = 0; *((s16 *) (new_var + 0x14)) = 0; *((s16 *) (new_var + 0x16)) = 0; } *((s16 *) (new_var + 0x18)) = 0; *((s16 *) (new_var + 0x1A)) = 0; } while (0);
  *((s16 *) (new_var + 0x1C)) = 0;
  *((s16 *) (new_var + 0x1E)) = 0;
}
