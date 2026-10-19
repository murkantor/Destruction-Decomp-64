#include "common.h"
#include "m2c.h"

typedef struct { s32 x[0x308 / 4]; } SaveData;
extern u8 D_80097D58;
extern s32 D_80097D50;
extern SaveData D_80097A10;
void Cont_ReadAll(void);
void func_80058B58(void);
s32 Pak_AllocateFile(s32, s32, s32 *);
s32 Pak_WriteFile(s32, s32, s32, s32, void *);

void Pak_SaveGame(s32 *res) {
    SaveData buf;
    SaveData *p;
    if (!D_80097D58) {
        Cont_ReadAll();
    }
    *res = Pak_AllocateFile(0, 0x320, &D_80097D50);
    if (!D_80097D58) {
        func_80058B58();
    }
    if (*res == 0) {
        buf = D_80097A10;
        p = &buf;
        if (!D_80097D58) {
            Cont_ReadAll();
        }
        *res = Pak_WriteFile(0, D_80097D50, 0, 0x320, p);
        if (!D_80097D58) {
            func_80058B58();
        }
    }
}
