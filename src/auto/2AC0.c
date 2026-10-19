#include "common.h"
#include "m2c.h"

s32 func_80001D78(s32);                             /* extern */
s32 HeapBlock_Ptr(M2C_UNK *);                       /* extern */
M2C_UNK Heap_AllocAlt(M2C_UNK *, M2C_UNK *, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK Heap_Free(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK func_80063BFC(s32, s32);                    /* extern */
M2C_UNK func_80064100(s32);                         /* extern */
extern M2C_UNK gMainHeap;

s32 func_80001EC0(s32 arg0) {
    M2C_UNK sp18;
    s32 temp_v0;
    s32 temp_v0_2;

    Heap_AllocAlt(&gMainHeap, &sp18, 0x3C, 7, 0);
    temp_v0_2 = HeapBlock_Ptr(&sp18);
    func_80063BFC(arg0, temp_v0_2);
    temp_v0 = func_80001D78(temp_v0_2);
    func_80064100(temp_v0);
    Heap_Free(&gMainHeap, &sp18);
    return temp_v0;
}

M2C_UNK Mem_Free();                            /* extern */

void func_80001F44(void) {
    Mem_Free();
}

M2C_UNK func_80063A20(s32);                         /* extern */

void func_80001F60(void) {
    s32 var_s0;

    var_s0 = 0x4E21;
    do {
        func_80063A20(var_s0);
        var_s0 += 1;
    } while (var_s0 < 0x4E2D);
}
