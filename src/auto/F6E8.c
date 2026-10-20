#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct Ui Ui;
typedef struct {
    u64 time;
    u8 flag;
    u8 pad9[3];
    f32 x;
    f32 y;
    s8 mode;
    u8 pad15[3];
} Blink;
typedef struct {
    u64 time;
    u64 time2;
    u8 flag;
    u8 pad11[3];
    f32 x;
    f32 y;
    s8 a;
    s8 b;
    u8 pad1E[2];
} BlinkElem;
typedef struct { u8 pad[0xD]; u8 xD; u8 padE[2]; s32 x10; } PInfo;
typedef struct { PInfo *info; u8 pad[0x64]; } PSlot;
typedef struct { s32 v[2][2][10]; } ElemTbl;
typedef struct { s32 elem; } Obj;
typedef struct { u8 pad[0x10A8]; u64 time; } Game;

extern Game *gGame;
extern Ui *gUi[];
extern Blink D_8007EEE8[];
extern BlinkElem D_8007EF68[][10];
extern PSlot D_800958F0[];
extern ElemTbl D_800B8290;
extern f64 D_800B8330, D_800B8338, D_800B8340;
extern u64 Time_Get(u64 *t);
extern void Ui_SetElementPos(Ui *ui, s32 type, s32 elem, f32 x, f32 y);
#ifdef TARGET_PC
#include "port_sret.h"
#else
extern void Ui_GetElementPos(Vec3f *out, Ui *ui, s32 type, s32 elem);
#endif
extern void Ui_InsertElementAfter(Ui *ui, s32 type, s32 elem, s32 after);
extern void Ui_RemoveElement(Ui *ui, s32 elem);
extern void func_8005DBDC(Obj *obj);

#define E D_8007EF68[p][i]
s32 Hud_UpdateBlinkElements(Obj *obj, s8 p)
{
    Vec3f pos;
    ElemTbl tbl = D_800B8290;
    u64 now;
    s32 elem;
    s32 i;
    f32 k;
    s32 a, b;
    s32 ea, eb;

    now = Time_Get(&gGame->time);
    elem = obj->elem;
    if (D_800958F0[p].info->xD) {
        Ui_SetElementPos(gUi[0], 2, elem, D_8007EEE8[p].x, D_8007EEE8[p].y);
        func_8005DBDC(obj);
        return 0;
    }
    if (D_8007EEE8[p].mode == 1 && now >= D_8007EEE8[p].time) {
        if (D_8007EEE8[p].flag) {
            Ui_SetElementPos(gUi[0], 2, elem, D_8007EEE8[p].x, 350.0f);
            D_8007EEE8[p].flag = 0;
        } else {
            Ui_SetElementPos(gUi[0], 2, elem, D_8007EEE8[p].x, D_8007EEE8[p].y);
            D_8007EEE8[p].flag = 1;
        }
        if (D_800958F0[p].info->x10 > 20) {
            k = 1.0f;
        } else {
            k = 0.15f;
            if (D_800958F0[p].info->x10 >= 11) {
                k = 0.5f;
            }
        }
        D_8007EEE8[p].time = now + (s32)(k * D_800B8330);
    }
    for (i = 0; i < 10; i++) {
        if (i == 8 || i == 9) {
            continue;
        }
        if (E.time2 == 0) {
            continue;
        }
        a = E.a;
        b = E.b;
        ea = tbl.v[p][a][i];
        eb = tbl.v[p][b][i];
        if ((now >= E.time2) & (a != -1)) {
            Ui_RemoveElement(gUi[0], ea);
            E.time = 0;
            E.time2 = 0;
            E.flag = 0;
            E.a = 2;
            E.b = 2;
            continue;
        }
        if (a != b) {
            Ui_InsertElementAfter(gUi[0], 2, elem, eb);
            if (a != -1) {
                if (a != 2) {
                    Ui_RemoveElement(gUi[0], ea);
                }
            } else {
                Ui_GetElementPos(&pos, gUi[0], 2, eb);
                E.x = pos.x;
                E.y = pos.y;
                Ui_GetElementPos(&pos, gUi[0], 2, elem);
                Ui_SetElementPos(gUi[0], 2, eb, E.x + pos.x, E.y + pos.y);
            }
            E.a = b;
        }
        Ui_GetElementPos(&pos, gUi[0], 2, elem);
        if (now >= E.time) {
            if (E.flag) {
                Ui_SetElementPos(gUi[0], 2, eb, pos.x + D_800B8338, pos.y + E.y);
                E.flag = 0;
            } else {
                Ui_SetElementPos(gUi[0], 2, eb, E.x + pos.x, E.y + pos.y);
                E.flag = 1;
            }
            E.time = now + D_800B8340;
        }
    }
    return 0;
}
