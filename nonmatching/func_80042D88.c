typedef struct { s32 id; s32 res; u8 pad8[0x14]; } UiRes;
typedef struct { u8 pad0[0x7588]; UiRes res[199]; u8 pad8B4C[0x10]; s16 nres; } Ui;
extern Ui *gUi[];
extern u8 D_801253F8;
extern u8 D_80097A0C[];
void func_80063AAC(s32);
s32 Res_Get(s32);
void Ui_SetElementPos(Ui *, s32, s32, f32, f32);



void func_80042D88(s32 arg0, s32 arg1) {
    f32 y0 = 78.0f;
    f32 y1 = y0;
    UiRes *r = NULL;
    s32 id;
    s32 i;

    if (arg1 != -1) {
        if (D_801253F8 || arg1 < 12) {
        } else if (arg1 < 12 || !D_80097A0C[arg1]) {
            arg1 = 0x18;
        }
        if (arg0 != -1) {
            if (D_801253F8 || arg0 < 12) {
            } else if (arg0 < 12 || !D_80097A0C[arg0]) {
                arg0 = 0x18;
            }
        }
        id = arg1 + 0x11371;
        func_80063AAC(id);
        for (i = 0; i < gUi[0]->nres; i++) {
            if (gUi[0]->res[i].id == id) {
                r = &gUi[0]->res[i];
                break;
            }
        }
        id = arg0 + 0x11371;
        r->res = Res_Get(id);
        r->id = id;
    }
    Ui_SetElementPos(gUi[0], 2, 10, 197.0f, y1);
    Ui_SetElementPos(gUi[0], 2, 11, 289.0f, y0);
}
