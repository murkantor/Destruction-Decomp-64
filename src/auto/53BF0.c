#include "common.h"
#include "m2c.h"

s32 RomFile_Read(M2C_UNK, s32, M2C_UNK, s32);      /* extern */
s32 Arc_SeekToEntryData();                                /* extern */

s32 Zip_ReadStoredEntry(void *arg0, void *arg1, M2C_UNK arg2) {
    s32 var_v0;

    var_v0 = Arc_SeekToEntryData();
    if (var_v0 == 0) {
        var_v0 = -(RomFile_Read(arg2, M2C_FIELD(arg1, s32 *, 0x14), 1, M2C_FIELD(arg0, s32 *, 4)) != 1);
    }
    return var_v0;
}
