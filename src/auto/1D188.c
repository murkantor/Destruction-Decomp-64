#include "common.h"
#include "m2c.h"

s32 Car_Create(s32);                                /* extern */
s32 HeapBlock_Ptr(M2C_UNK *);                       /* extern */
M2C_UNK Heap_AllocAlt(M2C_UNK *, M2C_UNK *, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK Heap_Free(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK Res_Load(s32, s32);                    /* extern */
M2C_UNK ObjList_Add(s32);                         /* extern */
extern M2C_UNK gMainHeap;

s32 Car_Load(s32 arg0) {
    M2C_UNK sp18;
    s32 temp_v0;
    s32 temp_v0_2;

    Heap_AllocAlt(&gMainHeap, &sp18, 0x2B0, 7, 0);
    temp_v0_2 = HeapBlock_Ptr(&sp18);
    Res_Load(arg0, temp_v0_2);
    temp_v0 = Car_Create(temp_v0_2);
    ObjList_Add(temp_v0);
    Heap_Free(&gMainHeap, &sp18);
    return temp_v0;
}
