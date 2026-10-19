#include "common.h"
#include "m2c.h"

M2C_UNK Heap_Free(M2C_UNK *, s32);                  /* extern */
M2C_UNK Mem_Free(s32);                              /* extern */
extern M2C_UNK gMainHeap;

void func_8001A328(s32 *arg0) {
    Heap_Free(&gMainHeap, *arg0 + 0x1C);
    Heap_Free(&gMainHeap, *arg0 + 0x30);
    Mem_Free(*arg0);
    *arg0 = 0;
}
