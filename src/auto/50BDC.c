#include "common.h"
#include "m2c.h"

#ifdef TARGET_PC
/* Bytes: see Pak_SaveGame (512E4.c) */
typedef struct {
    u8 w[0x308];
} SaveBuf;
#else
typedef struct {
    u32 w[0xC2];
} SaveBuf;
#endif
extern SaveBuf D_80097A10;
extern u8 D_80097D58;
extern s32 D_80097D50;
void Cont_ReadAll(void);
void Pad_StartRead(void);
s32 Pak_WriteFile(s32, s32, s32, s32, void *);

void Pak_WriteSave(s32 *out) {
#ifdef TARGET_PC
    /* Written from the global itself: see Pak_SaveGame */
    SaveBuf *p = &D_80097A10;
#else
    SaveBuf buf = D_80097A10;
    SaveBuf *p = &buf;
#endif

    if (!D_80097D58) {
        Cont_ReadAll();
    }
    *out = Pak_WriteFile(0, D_80097D50, 0, 0x320, p);
    if (!D_80097D58) {
        Pad_StartRead();
    }
}
