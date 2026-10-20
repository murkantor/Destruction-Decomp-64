#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s16 x8; u8 name[20]; u8 pad[2]; } SaveSlot;
typedef struct { u8 pad[0x10]; s32 x10; } Menu;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
extern s16 D_8011EEEA;
extern u8 D_800964D0;
extern SaveSlot gSaveSlots[];
u8 Pak_Open(s32 *);
void Cont_ReadAll(void);
void Pak_DeleteNote(s32, SaveSlot *);
void Pad_StartRead(void);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 Save_DeleteSlot(Menu *m) {
    s32 status;

    if (D_800964D0) {
        if (Pak_Open(&status) && (status == 0 || status == 5)) {
            Cont_ReadAll();
            Pak_DeleteNote(0, &gSaveSlots[m->x10]);
            gSaveSlots[m->x10].x0 = 0;
            gSaveSlots[m->x10].x4 = 0;
            gSaveSlots[m->x10].x8 = 0;
            gSaveSlots[m->x10].name[0] = 0;
            gSaveSlots[m->x10].name[1] = 0;
            gSaveSlots[m->x10].name[2] = 0;
            gSaveSlots[m->x10].name[3] = 0;
            gSaveSlots[m->x10].name[4] = 0;
            gSaveSlots[m->x10].name[5] = 0;
            gSaveSlots[m->x10].name[6] = 0;
            gSaveSlots[m->x10].name[7] = 0;
            gSaveSlots[m->x10].name[8] = 0;
            gSaveSlots[m->x10].name[9] = 0;
            gSaveSlots[m->x10].name[10] = 0;
            gSaveSlots[m->x10].name[11] = 0;
            gSaveSlots[m->x10].name[12] = 0;
            gSaveSlots[m->x10].name[13] = 0;
            gSaveSlots[m->x10].name[14] = 0;
            gSaveSlots[m->x10].name[15] = 0;
            gSaveSlots[m->x10].name[16] = 0;
            gSaveSlots[m->x10].name[17] = 0;
            gSaveSlots[m->x10].name[18] = 0;
            gSaveSlots[m->x10].name[19] = 0;
            Pad_StartRead();
        } else {
            func_80063380(0);
            Game_SetNextMode(2, 1);
        }
    }
    if (gGame->x10FC == D_8011EEEA) {
        Game_SetNextMode(3, 1);
    }
    return 1;
}
