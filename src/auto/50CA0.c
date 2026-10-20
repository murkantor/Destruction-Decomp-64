#include "common.h"
#include "m2c.h"

M2C_UNK Cont_ReadAll();                            /* extern */
M2C_UNK Pad_StartRead();                            /* extern */
M2C_UNK func_80059A94(M2C_UNK);                     /* extern */

void Pak_RepairId(void) {
    Cont_ReadAll();
    func_80059A94(0);
    Pad_StartRead();
}
