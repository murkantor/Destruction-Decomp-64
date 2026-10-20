#include "common.h"
#include "m2c.h"

typedef struct { u32 x0; s32 x4; u64 x8; s32 x10; s32 x14; s32 x18; char *x1C; } Credits;
typedef struct { char c[2]; } Str2;
typedef struct { u8 pad[0x10]; s8 x10; } Info;
typedef struct { u8 pad[0x28]; Info *x28; } Obj;
extern u8 *gGame;
extern void *gUi[];
extern Credits D_80124560;
extern Str2 D_800BAF30;
extern char gSaveSlotText[][0x32];
extern s32 D_80097324;
extern u8 D_800972F0;
extern const f64 D_800BB480;
extern const f64 D_800BB488;
extern const f64 D_800BB490;
extern const f64 D_800BB498;
u64 Time_Get(void *);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
char *strcpy(char *, const char *);
void Ui_EnableElementAlpha(void *, s32, s32, s32);
void Ui_DisableElementAlpha(void *, s32, s32);
void Ui_SetElementAlphaValue(void *, s32, s32, s32);
f32 func_8003EC70(char *, s32, f32);
void Ui_SetElementX(void *, s32, s32, f32);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);
void Res_Release(s32);

s32 Credits_Update(Obj *obj) {
    Str2 sfx = D_800BAF30;
    u64 now;
    s32 i;

    if (obj->x28->x10 != 0) {
        return 0;
    }
    now = Time_Get(gGame + 0x1058);
    if (now < D_80124560.x8) {
        return 0;
    }
    switch (D_80124560.x0) {
    case 0:
        Ui_RemoveElement(gUi[0], 0xA3);
        Ui_RemoveElement(gUi[0], 0xA5);
        Ui_AddElement(gUi[0], 2, 0xA4);
        D_80124560.x0 = 1;
        D_80124560.x8 = now;
        D_80124560.x18 = 0;
        D_80124560.x14 = 0;
        for (i = 0; i < 10; i++) {
            Ui_AddElement(gUi[0], 0, i + 0x17A);
            strcpy(gSaveSlotText[i], sfx.c);
            Ui_EnableElementAlpha(gUi[0], 0, i + 0x17A, 0);
        }
        break;
    case 1:
        if (D_80124560.x18 == 0) {
            if (D_80124560.x14 == 0) {
                for (i = 0; i < 10; i++) {
                    Ui_EnableElementAlpha(gUi[0], 0, i + 0x17A, 0);
                    if (D_80124560.x10 >= 0xBE) {
                        strcpy(gSaveSlotText[i], sfx.c);
                    } else {
                        strcpy(gSaveSlotText[i], D_80124560.x1C);
                        Ui_SetElementX(gUi[0], 0, i + 0x17A, func_8003EC70(gSaveSlotText[i], 0x111F8, 160.0f));
                        D_80124560.x1C += 0x28;
                        D_80124560.x10++;
                    }
                }
            }
            D_80124560.x14 += 0x30;
            if (D_80124560.x14 > D_80097324) {
                D_80124560.x14 = D_80097324;
                D_80124560.x18 = 1;
                now = (f64)now + D_800BB480;
                for (i = 0; i < 10; i++) {
                    Ui_DisableElementAlpha(gUi[0], 0, i + 0x17A);
                }
            }
        } else {
            if (D_80124560.x14 == D_80097324) {
                for (i = 0; i < 10; i++) {
                    Ui_EnableElementAlpha(gUi[0], 0, i + 0x17A, D_80097324);
                }
            }
            D_80124560.x14 -= 0x30;
            if (D_80124560.x14 < 0) {
                D_80124560.x14 = 0;
                D_80124560.x18 = 0;
                if (D_80124560.x10 >= 0xBE) {
                    D_80124560.x0 = 2;
                    D_80124560.x8 = (f64)now + D_800BB488;
                    for (i = 0; i < 10; i++) {
                        Ui_RemoveElement(gUi[0], i + 0x17A);
                    }
                }
            }
        }
        for (i = 0; i < 10; i++) {
            Ui_SetElementAlphaValue(gUi[0], 0, i + 0x17A, D_80124560.x14);
        }
        D_80124560.x8 = (f64)now + D_800BB490;
        break;
    case 2:
        if (D_800972F0) {
            Ui_AddElement(gUi[0], 2, 0xA6);
            D_80124560.x8 = (f64)now + D_800BB498;
            D_800972F0 = 0;
            break;
        }
        func_80063380(2);
        Game_SetNextMode(0x19, 1);
        Res_Release(0x11426);
        break;
    }
    return 1;
}
