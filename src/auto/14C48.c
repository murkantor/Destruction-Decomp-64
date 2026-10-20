#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[8]; s32 x8; u8 pad2[4]; } EntA;
typedef struct { s32 *x0; s32 x4[5]; s8 x18; s8 x19; u8 pad[0x28 - 0x1A]; } EntB;
typedef struct { u8 pad[0x18]; s32 x18[5]; s8 x2C; u8 pad2[3]; } EntC;
extern EntA *D_801255E4;
extern s32 D_80125F50;
extern EntB D_80124B20[];
extern s32 D_80131F28;
extern EntC *D_80125BD0;
extern s32 D_80124358;
extern s32 D_80131380;
extern void Res_Release(s32);
extern void Mem_Free(void *);

void Track_FreeAnims(void) {
    s32 i;
    s32 j;
    char unused[1];

    if (D_801255E4 != NULL) {
        for (i = 0; i < D_80125F50; i++) {
            Res_Release(D_801255E4[i].x8);
        }
        Mem_Free(D_801255E4);
        D_801255E4 = NULL;
    }
    for (i = 0; i < D_80131F28; i++) {
        *D_80124B20[i].x0 = D_80124B20[i].x4[D_80124B20[i].x19];
    }
    if (D_80125BD0 != NULL) {
        for (i = 0; i < D_80124358; i++) {
            for (j = 0; j < D_80125BD0[i].x2C; j++) {
                Res_Release(D_80125BD0[i].x18[j]);
            }
        }
        Mem_Free(D_80125BD0);
        D_80125BD0 = NULL;
    }
    D_80125F50 = 0;
    D_80124358 = 0;
    D_80131380 = 0;
    D_80131F28 = 0;
}
