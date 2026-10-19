#include "common.h"
#include "m2c.h"

M2C_UNK func_80019390(M2C_UNK *, s32, s32);         /* extern */
M2C_UNK func_80050D48(s32, s32 *, M2C_UNK *);       /* extern */
s32 Elf_FindSymbol(s32, s32);                        /* extern */
M2C_UNK Elf_Link(s32, M2C_UNK *);              /* extern */
s32 func_80063D8C();                                /* extern */
s32 func_80063DAC(s32);                             /* extern */
extern M2C_UNK D_80125460;
extern M2C_UNK func_80025438;

s32 func_80025350(s32 arg0) {
    s32 sp10;
    M2C_UNK sp14;
    s32 temp_s0;
    s32 temp_s0_2;

    temp_s0 = func_80063D8C();
    temp_s0_2 = func_80063DAC(arg0);
    func_80050D48(temp_s0, &sp10, &sp14);
    Elf_Link(sp10, &func_80025438);
    func_80019390(&D_80125460, Elf_FindSymbol(sp10, temp_s0_2), sp10);
    return 0;
}
