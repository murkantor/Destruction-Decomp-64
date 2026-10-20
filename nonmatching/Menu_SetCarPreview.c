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
/* agent65: size 0x1a0 (4 short), 9 words off. New fixes: 'register s32 r asm("$18") = x;' in fix()
 * puts 'move s2,a0' in the delay slot of the -1 test like the original, and
 * 'register f32 y0 asm("$f20")' fixes the f20/f22 swap. Left: the loop. In the original the gUi
 * load (pseudo 128) gets a0, k gets a1 and the pointer giv gets a2 via 'move a2,a0'. Here the giv
 * (allocated first, a2 because k prefers a0/a1) is copy-coalesced with 128 (copy 134 = 128 where 128
 * dies), so 128 takes a2 too and k gets a0. 128 must conflict with the giv (be live after its init)
 * or lose the copy preference. Tried: k as hard $5 (k copy moves before blez), u local in cond/body,
 * hard $4 u (no strength reduction, +8..16), n = nres local, if/do-while loop (+0x24), inline
 * FindRes(ui,id,r) (right size but r copies), GetUi() getters, dead branches around the loop. */
