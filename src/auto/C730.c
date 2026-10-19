#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y; } Pos2;
typedef struct { void *x0; u8 pad[0x64]; } Ent68;
extern s8 D_800B8180;
extern s8 D_800B8181;
extern Ent68 D_800958F0[];
extern char D_8007F77C[][4];
extern void *gUi[];
extern s32 D_800958A4;
extern s32 D_8007EED4;
extern s32 D_8007EED8[];
f32 func_8001FB40(void *);
s32 sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
Pos2 Ui_GetElementPos(void *, s32, s32);
f32 func_8003EC70(char *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

s32 func_8000BB30(s32 *arg0, s8 idx) {
    Pos2 pos;
    Pos2 unused;
    char sfx[2] = "\0";
    char *str;
    f32 val;
    s32 id;
    s32 font;
    void **ui;

    val = func_8001FB40(D_800958F0[idx].x0);
    str = D_8007F77C[idx];
    sprintf(str, "%d", (s32)(val * 2.232));
    strcat(str, (char *)sfx);
    id = *arg0;
    ui = gUi;
    pos = Ui_GetElementPos(ui[0], 0, id);
    if (D_800958A4 < 3) {
        pos.x = func_8003EC70(str, 0x111EC, D_8007EED4);
        Ui_SetElementPos(ui[0], 0, id, pos.x, pos.y);
    } else {
        pos.x = func_8003EC70(str, 0x111EC, D_8007EED8[idx]);
        Ui_SetElementPos(ui[0], 0, id, pos.x, pos.y);
    }
    return 0;
}
