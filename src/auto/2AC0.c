#include "common.h"
#include "m2c.h"

s32 Obj_CreateFromCfg(s32);                             /* extern */
s32 HeapBlock_Ptr(M2C_UNK *);                       /* extern */
M2C_UNK Heap_AllocAlt(M2C_UNK *, M2C_UNK *, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK Heap_Free(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK Res_Load(s32, s32);                    /* extern */
M2C_UNK ObjList_Add(s32);                         /* extern */
extern M2C_UNK gMainHeap;

s32 AiDriver_Load(s32 arg0) {
    M2C_UNK sp18[2]; /* a HeapBlock: Heap_AllocAlt writes 8 bytes */
    s32 temp_v0;
    s32 temp_v0_2;

    Heap_AllocAlt(&gMainHeap, &sp18, 0x3C, 7, 0);
    temp_v0_2 = HeapBlock_Ptr(&sp18);
    Res_Load(arg0, temp_v0_2);
    temp_v0 = Obj_CreateFromCfg(temp_v0_2);
    ObjList_Add(temp_v0);
    Heap_Free(&gMainHeap, &sp18);
    return temp_v0;
}

M2C_UNK Mem_Free();                            /* extern */

void AiDriver_Free(void *arg0) {
    Mem_Free(arg0);
}

M2C_UNK Res_Get(s32);                         /* extern */
#ifdef TARGET_PC
void Port_FieldLoadDrivers(void);
#endif

void AiDriver_LoadAll(void) {
    s32 var_s0;

    var_s0 = 0x4E21;
    do {
        Res_Get(var_s0);
        var_s0 += 1;
    } while (var_s0 < 0x4E2D);
#ifdef TARGET_PC
    Port_FieldLoadDrivers(); /* the doubled field: one for each extra car */
#endif
}
