#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern Game *gGame;
extern s16 D_8011EEEA;
extern void *gUi[];
extern char gSaveSlotText[];
u8 Pak_Open(s32 *);
void Pak_SaveGame(s32 *);
u8 Menu_HandleSelect(s32);
void func_80063564(s32, s32);
void func_80063380(s32);
void func_800100BC(s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
s32 sprintf(char *, const char *, ...);

s32 func_8003D950(Menu *m) {
    s32 pak;

    switch (m->x10) {
        case 0:
            func_80063564(3, 1);
            m->x10 = 0;
            break;
        case 1:
            if (Pak_Open(&pak)) {
            if (Menu_HandleSelect(pak)) {
                Pak_SaveGame(&pak);
                if (pak == 9) {
                    func_80063564(9, 1);
                    m->x10 = 0;
                }
                if (Menu_HandleSelect(pak) && gGame->x10FC == D_8011EEEA) {
                    func_80063380(2);
                    func_80063564(0x19, 1);
                    m->x10 = 0;
                }
            } else if (pak == 5) {
                Ui_SetElementPos(gUi[0], 0, 0x17A, 64.0f, 60.0f);
                Ui_SetElementPos(gUi[0], 2, 0x141, 95.0f, 59.0f);
                sprintf(gSaveSlotText, "YOUR                 IS STILL FULL");
                func_800100BC(m->x10 + 0x181, 0x111F8);
                m->x10 = 0;
                func_800100BC(m->x10 + 0x181, 0x111F4);
                Ui_RemoveElement(gUi[0], 0x130);
                Ui_AddElement(gUi[0], 2, 0x131);
            }
            } else {
                func_80063380(2);
                func_80063564(0x19, 1);
                m->x10 = 0;
            }
            break;
        case 2:
            func_80063564(8, 1);
            m->x10 = 0;
            break;
    }
    return 1;
}
