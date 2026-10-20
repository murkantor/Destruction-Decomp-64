#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 pad[3]; } Entry16;
extern void *gUi[];
extern s32 D_80097CC8;
extern Entry16 D_80095BE8[];
void Ui_ClearDrawList(void *);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
void Records_Draw(s32, s32, s32, s32);

void Menu_ShowVersus(s32 a, s32 b) {
    s32 typeA, typeB, idA, idB;
    s32 last = 13;

    if (b == last) {
        typeB = 3;
        typeA = 2;
        idB = 12;
        idA = D_80097CC8;
    } else if (b == 12) {
        idB = D_80097CC8;
        typeB = 2;
        if (a == last) {
            idA = 12;
            typeA = 3;
        } else {
            idA = D_80095BE8[a].x0;
            typeA = 0;
        }
    } else {
        idB = D_80095BE8[b].x0;
        idA = D_80095BE8[a].x0;
        typeB = idB >= 4;
        typeA = idA >= 4;
        if (a >= 12) {
            typeA = 2;
            idA = D_80097CC8;
        }
    }
    Ui_ClearDrawList(gUi[0]);
    Ui_AddElement(gUi[0], 2, 0xC);
    Ui_AddElement(gUi[0], 2, 0xD);
    Ui_AddElement(gUi[0], 2, 0x12F);
    Ui_AddElement(gUi[0], 2, 0x12E);
    Ui_AddElement(gUi[0], 2, 0x25);
    Ui_AddElement(gUi[0], 2, 0x26);
    Records_Draw(typeA, typeB, idA, idB);
    Ui_AddElement(gUi[0], 2, 0x9F);
    Ui_AddElement(gUi[0], 2, 0xA0);
    if (b == 0) {
        Ui_SetElementPos(gUi[0], 2, 0x9F, -20.0f, 73.0f);
    } else if (b == last) {
        Ui_SetElementPos(gUi[0], 2, 0xA0, -20.0f, 73.0f);
    } else {
        Ui_SetElementPos(gUi[0], 2, 0x9F, 197.0f, 73.0f);
        Ui_SetElementPos(gUi[0], 2, 0xA0, 289.0f, 73.0f);
    }
}
