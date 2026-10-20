#include "common.h"
#include "m2c.h"

typedef struct {
    u32 w[0xC2];
} SaveBuf;
extern SaveBuf D_80097A10;
extern u8 D_80097D58;
extern s32 D_80097D50;
void Cont_ReadAll(void);
void Pad_StartRead(void);
s32 Pak_WriteFile(s32, s32, s32, s32, void *);

void Pak_WriteSave(s32 *out) {
    SaveBuf buf = D_80097A10;
    SaveBuf *p = &buf;

    if (!D_80097D58) {
        Cont_ReadAll();
    }
    *out = Pak_WriteFile(0, D_80097D50, 0, 0x320, p);
    if (!D_80097D58) {
        Pad_StartRead();
    }
}
