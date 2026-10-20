#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */

void List_Push(void **arg0, s32 arg1) {
    void *temp_v0;
    void *temp_v1;

    temp_v0 = Mem_Alloc(8);
    M2C_FIELD(temp_v0, s32 *, 0) = arg1;
    M2C_FIELD(temp_v0, void **, 4) = NULL;
    temp_v1 = *arg0;
    if (temp_v1 != NULL) {
        M2C_FIELD(temp_v0, void **, 4) = temp_v1;
    }
    *arg0 = temp_v0;
}
