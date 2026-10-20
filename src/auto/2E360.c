#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */
void *Mem_AllocAligned(s32, M2C_UNK, M2C_UNK);         /* extern */
M2C_UNK Model_Init(void *, s32, M2C_UNK, M2C_UNK); /* extern */
s32 Res_Get(s32);                             /* extern */
M2C_UNK Res_Load(s32, void *);                 /* extern */
s32 Res_GetSize();                                /* extern */
void *Res_GetEnd(s32);                           /* extern */

void *func_8002D760(s32 arg0) {
    void *temp_a0;
    void *temp_v0;

    temp_v0 = Mem_AllocAligned(Res_GetSize(arg0), 0x3F, 0);
    Res_Load(arg0, temp_v0);
    M2C_FIELD(temp_v0, void **, 0x30) = Mem_Alloc(0x1C);
    M2C_FIELD(M2C_FIELD(temp_v0, void **, 0x30), void **, 0x18) = Res_GetEnd(Res_Get(M2C_FIELD(temp_v0, s32 *, 0x24)));
    temp_a0 = M2C_FIELD(temp_v0, void **, 0x30);
    Model_Init(temp_a0, M2C_FIELD(M2C_FIELD(temp_a0, void **, 0x18), s32 *, 0x20), 0, 0);
    return temp_v0;
}

M2C_UNK Mem_Free(void *);                      /* extern */
M2C_UNK Res_Release(s32);                         /* extern */

void func_8002D7E4(void *arg0) {
    Res_Release(M2C_FIELD(arg0, s32 *, 0x24));
    Mem_Free(M2C_FIELD(arg0, void **, 0x30));
    Mem_Free(arg0);
}
