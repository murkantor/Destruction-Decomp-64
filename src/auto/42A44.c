#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x334]; u8 x334[4]; } Game;
extern Game *gGame;
extern void *gUi;
extern s32 D_800958A8;
extern char gSaveSlotText[][50];
extern char D_8009694C[][6][16];
extern u8 D_80096E6E;
extern u8 D_801253F8;
extern u8 D_80097A0C[];
extern s32 D_800964F0[];
extern s32 D_80096500[];
void func_80042114(void);
void Menu_SetCarPreview(s32, s32);
char *strcpy(char *, const char *);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void ObjList_Remove(void *, s32);
s32 func_8003E644(s32, s32);

#define LOCKED(x) ((x) != -1 && !(D_801253F8 != 0 || (x) < 12) && ((x) < 12 || D_80097A0C[x] == 0))

void TrackSelect_Update(s32 cur, s32 prev, s32 slot) {
    s32 k;
    s32 i;

    if (D_800958A8 != 3) {
        func_80042114();
        Menu_SetCarPreview(cur, prev);
        if (cur < 12) {
            k = 0;
        } else {
            k = cur - 11;
        }
        strcpy(gSaveSlotText[0], D_8009694C[k][0]);
        strcpy(gSaveSlotText[1], D_8009694C[k][1]);
        strcpy(gSaveSlotText[2], D_8009694C[k][2]);
        strcpy(gSaveSlotText[3], D_8009694C[k][3]);
        strcpy(gSaveSlotText[4], D_8009694C[k][4]);
        strcpy(gSaveSlotText[5], D_8009694C[k][5]);
        if (D_80096E6E == 0) {
            if (!LOCKED(cur)) {
                if (LOCKED(prev)) {
                    Ui_AddElement(gUi, 0, 0x180);
                }
            } else {
                if (!LOCKED(prev)) {
                    Ui_RemoveElement(gUi, 0x180);
                }
            }
        } else if (LOCKED(cur)) {
            Ui_RemoveElement(gUi, 0x8F);
            for (i = 0; i < 6; i++) {
                Ui_RemoveElement(gUi, 0x17A + i);
            }
            D_80096E6E = 0;
        }
    }
    if (prev != -1) {
        ObjList_Remove(gGame->x334, D_800964F0[slot]);
        D_80096500[slot] = D_800964F0[slot];
        D_800964F0[slot] = 0;
    }
    D_800964F0[slot] = func_8003E644(cur, slot);
}
