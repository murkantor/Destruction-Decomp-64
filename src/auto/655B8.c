#include "common.h"
#include "m2c.h"

extern s32 D_80095904;
extern s32 D_80095908;
extern s32 D_8009596C;
extern s32 D_80095970;
extern s32 D_800959D4;
extern s32 D_800959D8;
extern s32 D_80095A3C;
extern s32 D_80095A40;

void Players_SyncTeammate(void *arg0) {
    s8 temp_v1;

    temp_v1 = M2C_FIELD(M2C_FIELD(arg0, void **, 0x28), s8 *, 0x10);
    switch (temp_v1) {                              /* irregular */
    case 0:
        D_8009596C = M2C_FIELD(arg0, s32 *, 0xC);
        D_80095970 = M2C_FIELD(arg0, s32 *, 0x10);
        return;
    case 1:
        D_80095904 = M2C_FIELD(arg0, s32 *, 0xC);
        D_80095908 = M2C_FIELD(arg0, s32 *, 0x10);
        return;
    case 2:
        D_80095A3C = M2C_FIELD(arg0, s32 *, 0xC);
        D_80095A40 = M2C_FIELD(arg0, s32 *, 0x10);
        return;
    case 3:
        D_800959D4 = M2C_FIELD(arg0, s32 *, 0xC);
        D_800959D8 = M2C_FIELD(arg0, s32 *, 0x10);
        return;
    }
}
