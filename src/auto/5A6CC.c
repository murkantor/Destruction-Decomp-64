#include "common.h"
#include "m2c.h"

M2C_UNK osPfsFileState(void *, M2C_UNK, M2C_UNK);
extern char gPakFs;

void Pak_FileState(s32 arg0, M2C_UNK arg1, M2C_UNK arg2)
{
  osPfsFileState((arg0 * 0x68) + (&gPakFs), arg2, arg1);
}
