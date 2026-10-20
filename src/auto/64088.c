#include "common.h"
#include "m2c.h"

M2C_UNK func_8002D088(void *);                      /* extern */
M2C_UNK Clock_Init(void *, M2C_UNK *);           /* extern */
M2C_UNK Clock_Resume(s32);                         /* extern */
M2C_UNK ObjList_Init(void *);                      /* extern */
s16 Obj_NextId();                                /* extern */
extern M2C_UNK gGameTime;
extern s32 gGame;

s32 func_80063488(void *arg0, s16 arg1) {
    M2C_FIELD(arg0, s16 *, 0) = 0x3E9;
    M2C_FIELD(arg0, s16 *, 2) = Obj_NextId();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s16 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10F8) = 0;
    Clock_Init(arg0 + 0x1058, &gGameTime);
    Clock_Resume(gGame + 0x1058);
    func_8002D088(arg0);
    ObjList_Init(arg0 + 0x334);
    return 0;
}
