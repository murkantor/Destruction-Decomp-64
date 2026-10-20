#include "common.h"
#include "m2c.h"

typedef struct { s32 id; s32 res; u8 pad8[0x14]; } UiRes;
typedef struct { u8 pad0[0x7588]; UiRes res[199]; u8 pad8B4C[0x10]; s16 nres; } Ui;
extern Ui *gUi[];
extern u8 D_801253F8;
extern u8 D_80097A0C[];
void Res_Release(s32);
s32 Res_Get(s32);
void Ui_SetElementPos(Ui *, s32, s32, f32, f32);



static inline s32 fix(s32 x) {
    register s32 r asm("$18") = x;
    if (x != -1) {
        if (D_801253F8 || x < 12) {
        } else if (x >= 12 && D_80097A0C[x]) {
            return r;
        } else {
            r = 0x18;
        }
    }
    return r;
}

void Menu_SetCarPreview(s32 arg0, s32 arg1) {
    register f32 y0 asm("$f20") = 78.0f;
    f32 y1 = y0;
    UiRes *r = NULL;
    s32 id;
    s32 i;
    s32 a0;

    if (arg1 != -1) {
        if (D_801253F8 || arg1 < 12) {
        } else if (arg1 < 12 || !D_80097A0C[arg1]) {
            arg1 = 0x18;
        }
        a0 = fix(arg0);
        id = arg1 + 0x11371;
        Res_Release(id);
        for (i = 0; i < gUi[0]->nres; i++) {
            if (arg1 + 0x11371 == gUi[0]->res[i].id) {
                r = &gUi[0]->res[i];
                break;
            }
        }
        id = 0x11371;
        id += a0;
        r->res = Res_Get(id);
        r->id = id;
    }
    Ui_SetElementPos(gUi[0], 2, 10, 197.0f, y0);
    Ui_SetElementPos(gUi[0], 2, 11, 289.0f, y1);
}
/* agent67: matches. The loop compares against 'arg1 + 0x11371' written out again instead of a
 * copy k = id. Loop.c hoists it as an invariant after the loop test and cse2 turns it into the
 * 'move a1,s0' copy, which frees a0 for the gUi load and gives the giv 'move a2,a0'. */
