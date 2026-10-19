#include "common.h"
#include "m2c.h"

M2C_UNK Heap_Free(M2C_UNK *, s32);                  /* extern */
M2C_UNK Mem_Free(s32);                              /* extern */
extern M2C_UNK gMainHeap;

void func_80015F28(s32 *arg0) {
    s32 temp_s1;
    s32 temp_s1_2;

    temp_s1 = *arg0;
    Heap_Free(&gMainHeap, temp_s1 + 0xC);
    Heap_Free(&gMainHeap, temp_s1 + 0x14);
    temp_s1_2 = *arg0;
    Heap_Free(&gMainHeap, temp_s1_2 + 0x114);
    Heap_Free(&gMainHeap, temp_s1_2 + 0x11C);
    Mem_Free(*arg0);
    *arg0 = 0;
}
