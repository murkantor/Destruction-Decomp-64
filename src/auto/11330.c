#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */
M2C_UNK func_800168C0(void *, M2C_UNK);             /* extern */
M2C_UNK func_800170F0(void *, M2C_UNK);             /* extern */
M2C_UNK Particles_InitTrail(void *, M2C_UNK);             /* extern */
s16 func_80064D70();                                /* extern */

void *ParticleSys_Create(void) {
    void *temp_v0;

    temp_v0 = Mem_Alloc(0x1E0);
    M2C_FIELD(temp_v0, s16 *, 0) = 0x3EF;
    M2C_FIELD(temp_v0, s16 *, 2) = func_80064D70();
    M2C_FIELD(temp_v0, void **, 4) = temp_v0;
    func_800168C0(temp_v0 + 0xB0, 0xC8);
    func_800170F0(temp_v0 + 0x148, 0xC8);
    Particles_InitTrail(temp_v0 + 0xC, 0x32);
    return temp_v0;
}

M2C_UNK func_800169E8(s32);                         /* extern */
M2C_UNK func_80017220(s32);                         /* extern */
M2C_UNK func_800175F4(s32);                         /* extern */

void func_80010794(s32 arg0) {
    func_80017220(arg0 + 0x148);
    func_800169E8(arg0 + 0xB0);
    func_800175F4(arg0 + 0xC);
}

M2C_UNK func_80017058(s32);                         /* extern */
M2C_UNK func_8001742C(s32);                         /* extern */
M2C_UNK func_80017930(s32);                         /* extern */
M2C_UNK Mem_Free(s32);                         /* extern */

void ParticleSys_Destroy(s32 arg0) {
    func_8001742C(arg0 + 0x148);
    func_80017058(arg0 + 0xB0);
    func_80017930(arg0 + 0xC);
    Mem_Free(arg0);
}
