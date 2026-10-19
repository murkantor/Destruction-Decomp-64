#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; } Ent1C83C;
typedef struct { u8 pad[0x16]; u8 x16; u8 pad17; Ent1C83C *x18; } Obj1C83C;
typedef struct { s32 x0[5]; s8 x14; u8 pad[0x1B]; } Grp1C83C;
extern u8 D_80091B28;
extern Grp1C83C D_80091B44[];
Obj1C83C *Res_GetEnd(void *);
void func_80063AAC(s32);
void func_80063B54(s32);
void Mem_Free(void *);

void Model_Free(void *arg0) {
    Obj1C83C *obj;
    Grp1C83C *g;
    s32 i;
    s32 k;

    obj = Res_GetEnd(arg0);
    if (obj != NULL) {
        if (!D_80091B28) {
            D_80091B28 = 1;
            for (k = 0; k < 7; k++) {
                for (i = 0; i < D_80091B44[k].x14; i++) {
                    func_80063AAC(D_80091B44[k].x0[i]);
                }
            }
        }
        if (obj->x16 != 0) {
            for (i = 0; i < obj->x16; i++) {
                func_80063B54(obj->x18[i].x0);
            }
        }
        Mem_Free(arg0);
    }
}
