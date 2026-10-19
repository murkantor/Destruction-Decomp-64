#include "common.h"
#include "m2c.h"

M2C_UNK func_80059278();                            /* extern */
M2C_UNK func_8005C5EC(M2C_UNK *);                   /* extern */
M2C_UNK func_8005D5C4(M2C_UNK *);                   /* extern */
M2C_UNK func_8005FF68(M2C_UNK *);                   /* extern */
M2C_UNK func_80062E4C();                            /* extern */
M2C_UNK func_80062E90();                            /* extern */
M2C_UNK func_80063070(void *);                      /* extern */
M2C_UNK func_80063404(M2C_UNK *);                   /* extern */
M2C_UNK func_80063488(M2C_UNK *, s16);              /* extern */
M2C_UNK func_800635D0(void *);                      /* extern */
M2C_UNK func_80063890(s32);                         /* extern */
M2C_UNK func_800638C0(s32);                         /* extern */
extern M2C_UNK D_801233B0;
extern M2C_UNK D_801245B0;
extern M2C_UNK D_80126240;
extern M2C_UNK D_8012F030;

void func_800631E0(void *arg0) {
    s32 temp_s0;

    temp_s0 = arg0 + 0x10;
    func_80063890(temp_s0);
    func_800638C0(temp_s0);
    func_80059278();
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD30) = &D_801245B0;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD38) = &D_801233B0;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD24) = &D_8012F030;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD34) = &D_80126240;
    func_8005FF68(M2C_FIELD(arg0, M2C_UNK **, 0x4BD30));
    func_8005C5EC(M2C_FIELD(arg0, M2C_UNK **, 0x4BD38));
    func_8005D5C4(M2C_FIELD(arg0, M2C_UNK **, 0x4BD34));
    func_80063488(M2C_FIELD(arg0, M2C_UNK **, 0x4BD24), M2C_FIELD(arg0, s16 *, 0x4BD28));
    func_80063404(M2C_FIELD(arg0, M2C_UNK **, 0x4BD24));
    func_80062E4C();
    func_80063890(temp_s0);
loop_1:
    func_80062E90();
    func_800635D0(arg0 + 0x10);
    func_80063070(arg0);
    goto loop_1;
}
