typedef struct { s32 id; s32 res; u8 pad8[0x14]; } UiRes;
typedef struct { u8 pad0[0x7588]; UiRes res[199]; u8 pad8B4C[0x10]; s16 nres; } Ui;
extern Ui *gUi[];
extern u8 D_801253F8;
extern u8 D_80097A0C[];
void Res_Release(s32);
s32 Res_Get(s32);
void Ui_SetElementPos(Ui *, s32, s32, f32, f32);



static inline s32 fix(s32 x) {
    s32 r = x;
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
    f32 y0 = 78.0f;
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
        { s32 k = id;
        for (i = 0; i < gUi[0]->nres; i++) {
            if (gUi[0]->res[i].id == k) {
                r = &gUi[0]->res[i];
                break;
            }
        } }
        id = 0x11371;
        id += a0;
        r->res = Res_Get(id);
        r->id = id;
    }
    Ui_SetElementPos(gUi[0], 2, 10, 197.0f, y0);
    Ui_SetElementPos(gUi[0], 2, 11, 289.0f, y1);
}
/* agent55: score 33 (ds). The inline fix() with 'r = x' and an early 'return r' in the D_80097A0C branch recreates the j before the loop, so loop.c now puts the found block where the original has it; id = 0x11371; id += a0 builds the constant in s0. Still wrong: fix()'s r lands in a0 and is copied to s2 at the join (the original copies arg0 to s2 before the -1 test), the loop's id copy is in a0 rather than a1 (so there is no gUi copy into a2), and f20/f22 are swapped on the two calls. Tried: if/else, goto and shared-label forms of the fixup, 13 inline bodies x 3 call forms, y0/y1 init forms, local u/k/n copies in the loop. */
/* agent60: unchanged (size 0x1a0, 4 short). Tried: a second fixup written like the first (a0 = arg0
 * then the same if/else, both forms: -8/-4 bytes), every placement of y1 = y0, y0/y1 decl orders, two
 * 78.0f constants, swapped call args (f20/f22 order does not follow the source), y[2] array.
 * Prototypes checked (Res_Release(s32), Res_Get(s32)). Idea not tried: the loop as an inline
 * FindRes(ui, id) returning an index, given the a0/a1 param-style copies (move a1,s0 / move a2,a0). */
