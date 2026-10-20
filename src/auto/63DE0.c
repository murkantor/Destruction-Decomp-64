#include "common.h"
#include "m2c.h"

M2C_UNK Rumble_AllowRetry();                            /* extern */
M2C_UNK World_Init(M2C_UNK *);                   /* extern */
M2C_UNK Ui_Init(M2C_UNK *);                   /* extern */
M2C_UNK Render_Init(M2C_UNK *);                   /* extern */
M2C_UNK Timer_Reset();                            /* extern */
M2C_UNK Pad_LatchAll();                            /* extern */
M2C_UNK Game_Frame(void *);                      /* extern */
M2C_UNK Game_StartMode(M2C_UNK *);                   /* extern */
M2C_UNK func_80063488(M2C_UNK *, s16);              /* extern */
M2C_UNK Clock_Update(void *);                      /* extern */
M2C_UNK Clock_Reset(s32);                         /* extern */
M2C_UNK Clock_Resume(s32);                         /* extern */
extern M2C_UNK D_801233B0;
extern M2C_UNK D_801245B0;
extern M2C_UNK D_80126240;
extern M2C_UNK D_8012F030;

void Game_MainLoop(void *arg0) {
    s32 temp_s0;

    temp_s0 = arg0 + 0x10;
    Clock_Reset(temp_s0);
    Clock_Resume(temp_s0);
    Rumble_AllowRetry();
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD30) = &D_801245B0;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD38) = &D_801233B0;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD24) = &D_8012F030;
    M2C_FIELD(arg0, M2C_UNK **, 0x4BD34) = &D_80126240;
    Render_Init(M2C_FIELD(arg0, M2C_UNK **, 0x4BD30));
    World_Init(M2C_FIELD(arg0, M2C_UNK **, 0x4BD38));
    Ui_Init(M2C_FIELD(arg0, M2C_UNK **, 0x4BD34));
    func_80063488(M2C_FIELD(arg0, M2C_UNK **, 0x4BD24), M2C_FIELD(arg0, s16 *, 0x4BD28));
    Game_StartMode(M2C_FIELD(arg0, M2C_UNK **, 0x4BD24));
    Timer_Reset();
    Clock_Reset(temp_s0);
loop_1:
    Pad_LatchAll();
    Clock_Update(arg0 + 0x10);
    Game_Frame(arg0);
    goto loop_1;
}
