#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */
s32 Ui_FindElement();                               /* extern */

void func_8005D8B8(void *arg0) {
    s32 temp_s1;
    void *temp_v0;

    temp_s1 = Ui_FindElement(arg0);
    temp_v0 = Mem_Alloc(8);
    if (temp_v0 != NULL) {
        M2C_FIELD(temp_v0, s32 *, 0) = temp_s1;
        M2C_FIELD(temp_v0, void **, 4) = (void *) M2C_FIELD(arg0, void **, 0x8B54);
        M2C_FIELD(arg0, void **, 0x8B54) = temp_v0;
    }
}
