#include "common.h"
#include "m2c.h"

M2C_UNK osPfsFileState(void *, M2C_UNK, M2C_UNK);
extern char gPakFs;

#ifdef TARGET_PC
/* Returns osPfsFileState's result: on the N64 it stays in v0 for the caller
   (Pak_RefreshSaveSlots uses it); on a PC nothing keeps it unless returned */
s32 Pak_FileState(s32 arg0, M2C_UNK arg1, M2C_UNK arg2)
{
  return (s32)osPfsFileState((arg0 * 0x68) + (&gPakFs), arg2, arg1);
}
#else
void Pak_FileState(s32 arg0, M2C_UNK arg1, M2C_UNK arg2)
{
  osPfsFileState((arg0 * 0x68) + (&gPakFs), arg2, arg1);
}
#endif
