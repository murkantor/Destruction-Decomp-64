#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[0x10]; s32 x14; u8 pad18[8]; } ResEntry;
typedef struct { u8 pad0[0x14]; s32 off; u8 pad18[0x10]; s32 size; u8 pad2C[0x14]; } ResEntry2;
extern s32 gNumResTypes;
extern ResEntry D_80097D94[];
extern ResEntry D_80097DA8[];
extern u8 gResArchiveEnd[];
static inline s32 diff(s32 a, s32 b) { return a - b; }
void Res_ComputeSizes(void) {
    s32 i, j;
    char unused[1];
    for (i = 0; i < gNumResTypes - 1; i = j) {
        j = i + 1;
        D_80097DA8[i].x0 = diff(D_80097D94[j].x0, D_80097D94[i].x0);
    }
    D_80097DA8[i].x0 = (s32)gResArchiveEnd - D_80097D94[i].x0;
}
