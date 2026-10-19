#include "common.h"
#include "m2c.h"

M2C_UNK Heap_Free(M2C_UNK *, s32);                  /* extern */
M2C_UNK Mem_Free(s32);                              /* extern */
extern M2C_UNK gMainHeap;

void func_80018B94(s32 *arg0) {
    s32 temp_s1;
    s32 temp_s1_2;

    Heap_Free(&gMainHeap, *arg0 + 0x21C);
    temp_s1 = *arg0;
    Heap_Free(&gMainHeap, temp_s1 + 0x14);
    Heap_Free(&gMainHeap, temp_s1 + 0x1C);
    temp_s1_2 = *arg0;
    Heap_Free(&gMainHeap, temp_s1_2 + 0x11C);
    Heap_Free(&gMainHeap, temp_s1_2 + 0x124);
    Mem_Free(*arg0);
    *arg0 = 0;
}
