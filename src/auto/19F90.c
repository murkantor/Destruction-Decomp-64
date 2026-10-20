#include "common.h"
#include "m2c.h"

typedef struct {
    u8 op;
    u8 pad1[3];
    s32 x4;
} Cmd;

typedef struct {
    u8 pad0[0x4];
    Cmd *x4;
    s32 x8;
    u8 *xC;
    s32 x10;
} Script;

typedef struct {
    u8 pad0[0x4];
    s32 x4;
    u8 pad8[0x4];
    s16 xC;
    s8 xE;
    s8 xF;
} SfxDef;

typedef struct {
    u8 pad0[0x4];
    s32 x4[10];
    s8 x2C;
    u8 pad2D;
    s16 x2E;
} Group;

typedef struct {
    u8 pad0[0x60];
    s32 x60;
    SfxDef *x64;
    s32 x68;
    Group *x6C;
} Race;

typedef struct {
    u8 pad0[0x1100];
    Race *race;
} Game;

typedef struct {
    u8 pad0[0x10C];
    s32 x10C;
    Script *x110;
    s32 x114;
} Obj;

extern Game *gGame[];
extern u8 D_800958A1[];
extern s32 D_800958B4[];
void Grid_Create(s32 *, s32, s32, s32);
void Grid_AddItem(s32, s32);
void Grid_BuildCellsXZ(s32, u8 *);
s32 Res_Get(s32);
void Tex_AddAnim(s32 *, s32 *, s8, s8, s16);
void Tex_AddScroll(Cmd *, s16, s8, s8);

void Track_RelocateDisplayList(Obj *arg0, Script *arg1, s32 arg2) {
    SfxDef *sfx;
    Group *groups;
    u8 flag;
    s32 numGroups;
    s32 numSfx;
    u8 found;
    s32 foundIdx;
    s32 i;
    s32 j;
    s32 k;
    Cmd *c;
    u8 cont;
    Race *race;

    arg0->x110 = arg1;
    arg0->x114 = arg2;
    flag = D_800958A1[0];
    race = gGame[0]->race;
    sfx = race->x64;
    groups = race->x6C;
    numGroups = race->x68;
    numSfx = race->x60;
    arg0->x10C = 0;
    found = 0;
    foundIdx = 0;
    if (D_800958B4[0] == 2 || D_800958B4[0] == 11) {
        Grid_Create(&arg0->x10C, 0x6590, 100, 100);
        for (i = 0; i < arg0->x110->x10; i++) {
            Grid_AddItem(arg0->x10C, i);
        }
        Grid_BuildCellsXZ(arg0->x10C, arg0->x110->xC);
    }
    for (i = 0; i < arg1->x8; i++) {
        c = &arg1->x4[i];
        switch ((u32)c->op) {
        case 0xFD:
            c->x4 = Res_Get(c->x4);
            cont = 1;
            for (j = 0; j < numGroups && cont; j++) {
                for (k = 0; cont && k < groups[j].x2C; k++) {
                    if (c->x4 == groups[j].x4[k]) {
                        Tex_AddAnim(&c->x4, groups[j].x4, k, groups[j].x2C, groups[j].x2E);
                        cont = 0;
                    }
                }
            }
            for (j = 0; j < numSfx; j++) {
                if (c->x4 == sfx[j].x4) {
                    found = 1;
                    foundIdx = j;
                    break;
                }
            }
            break;
        case 1:
            c->x4 = (s32)(arg1->xC + c->x4 * 16);
            break;
        case 0xD9:
            if (flag) {
                s32 t = (c->x4 & 0x400) == 0;
                if (!(c->x4 & 0x200) ? (t ^ 1) != 0 : t != 0) {
                    c->x4 ^= 0x600;
                }
            }
            break;
        case 0xF2:
            if (found) {
                found = 0;
                Tex_AddScroll(c, sfx[foundIdx].xC, sfx[foundIdx].xE, sfx[foundIdx].xF);
            }
            break;
        case 0xDE:
            if (c->x4 >= 0) {
                c->x4 = (s32)&arg1->x4[c->x4];
            }
            break;
        }
    }
}
