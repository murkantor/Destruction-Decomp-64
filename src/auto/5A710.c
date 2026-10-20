#include "common.h"
#include "m2c.h"

M2C_UNK osPfsDeleteFile(void *, u16, s32, void *, void *);
extern volatile char gPakFs;

void Pak_DeleteNote(s32 arg0, void *arg1)
{
  osPfsDeleteFile((arg0 * 0x68) + (&gPakFs), *((u16 *) (((s8 *) arg1) + 8)), *((s32 *) (((s8 *) arg1) + 4)), arg1 + 0xE, arg1 + 0xA);
}
