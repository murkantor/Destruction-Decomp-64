#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[4];
    u8 *widths;
    u8 pad8[0xC];
    u8 count;
    u8 pad15[3];
    u8 *chars;
    u8 pad1C[4];
} FontData;

typedef struct {
    s32 id;
    FontData data;
} Font;

typedef struct {
    u8 pad[0x8938];
    Font fonts[14];
    u8 pad2[0x8B62 - 0x8938 - 14 * 0x24];
    s16 numFonts;
} Ui;

typedef struct { s32 key; s32 value; } KeyValue;

extern Ui *gUi;
extern KeyValue D_80137104[];
void Res_Load(s32, void *);
void Ui_AddResource(Ui *, void *);

s32 Font_GetStringWidth(u8 *str, s32 fontId) {
    s8 i;
    s8 j;
    s32 width = 0;
#ifdef TARGET_PC
    PORT_DMA_BUF(u8, buf, 0x2440); /* 0x28 bytes */
#else
    u8 buf[0x28];
#endif
    FontData *fd;
    s32 k;
    u8 *p;

    for (i = 0; i < gUi->numFonts; i++) {
        if (gUi->fonts[i].id == fontId) {
            break;
        }
    }
    if (i == gUi->numFonts) {
        for (j = 0; D_80137104[j].key != 0; j++) {
            if (D_80137104[j].value == fontId) {
                break;
            }
        }
        Res_Load(D_80137104[j].key, buf);
        Ui_AddResource(gUi, buf);
    }
    fd = &gUi->fonts[i].data;
    for (p = str; *p != 0; p++) {
        for (k = 0; k < fd->count; k++) {
            if (*p == fd->chars[k]) {
                width += fd->widths[k];
                break;
            }
        }
    }
    return width;
}

typedef struct { u8 pad[0xC]; s16 xC; u8 pad2[0x10FC - 0xE]; s16 x10FC; } Game;
extern Game *gGame;
extern s32 D_8007F224, D_8007F220, D_8007F214, D_8007F210, D_8007F21C, D_8007F218;
extern s32 D_8007F1FC, D_8007F1F8;
extern s32 D_8007F208[], D_8007F200[], D_8007F1F0[], D_8007F1E8[], D_8007F238[];
extern u32 D_800958B4;
extern u32 D_800958A8; extern s32 D_800958A4[], D_800958AC[];

typedef struct { s32 x0; s32 x4; } Pair;
void Hud_GetLayoutIds(Pair *out) {
    s32 idx;
    switch (gGame->xC) {
    case -1:
        break;
    case 0:
        out->x0 = D_8007F224;
        out->x4 = D_8007F220;
        break;
    case 1:
        out->x0 = D_8007F214;
        out->x4 = D_8007F210;
        break;
    case 2:
        if (gGame->x10FC == 0x19) {
            out->x0 = D_8007F21C;
            out->x4 = D_8007F218;
        } else {
            idx = D_800958B4 >= 4;
            switch (D_800958A8) {
            case 0:
                out->x0 = D_8007F208[idx];
                out->x4 = D_8007F200[idx];
                break;
            case 1:
                out->x0 = D_8007F1F0[idx];
                out->x4 = D_8007F1E8[idx];
                break;
            case 2:
                out->x0 = D_8007F1FC;
                out->x4 = D_8007F1F8;
                break;
            case 3:
                out->x0 = D_8007F238[D_800958A4[0] * 4 + D_800958AC[0]];
                out->x4 = D_8007F208[D_800958A4[0] * 4 + D_800958AC[0]];
                break;
            }
        }
        break;
    }
}
