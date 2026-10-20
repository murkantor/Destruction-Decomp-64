#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s16 x8; u8 xA[4]; u8 xE[16]; u8 pad[2]; } SaveSlot;
extern SaveSlot gSaveSlots[];
extern u8 D_80097D58;
extern s32 D_80097D80;
void Cont_ReadAll(void);
void Pad_StartRead(void);
s32 Pak_FileState(s32, SaveSlot *, s32);
void Pak_FreeBlocks(s32, s32 *);

void Pak_RefreshSaveSlots(void) {
    s32 i;
    s32 j;

    if (!D_80097D58) {
        Cont_ReadAll();
    }
    for (i = 0; i < 16; i++) {
        if (Pak_FileState(0, &gSaveSlots[i], i)) {
            gSaveSlots[i].x0 = 0;
            gSaveSlots[i].x4 = 0;
            gSaveSlots[i].x8 = 0;
            for (j = 3; j >= 0; j--) {
                gSaveSlots[i].xA[j] = 0;
            }
            for (j = 15; j >= 0; j--) {
                gSaveSlots[i].xE[j] = 0;
            }
        }
    }
    Pak_FreeBlocks(0, &D_80097D80);
    if (!D_80097D58) {
        Pad_StartRead();
    }
}
