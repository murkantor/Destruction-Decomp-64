typedef struct { f32 x, y, z; } Vec3f;
typedef struct { s32 x0; u8 pad4[0x14]; f32 x18; f32 x1C; } Marker;
typedef struct { u8 pad0[0xD]; u8 xD; u8 padE[0x3A]; Marker *x48; } Car;
typedef struct { u8 pad0[0x74]; Car *x74[12]; u8 padA4_[0xA4 - 0x74 - 48]; s32 xA4; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
typedef struct { s32 x0; u8 pad4[0x7C]; s32 x80; s32 x84; s32 x88; s32 x8C; } Widget;
typedef struct UiNode { Widget *x0; struct UiNode *x4; } UiNode;
typedef struct { u8 pad[0x8B54]; UiNode *x8B54; } Ui;
typedef struct {
    u8 pad0[8];
    f32 x8;
    f32 xC;
    f32 x10;
    f32 x14;
    f32 x18;
    f32 x1C;
    f32 x20;
    f32 x24;
    u8 pad28[0x20];
} MapCfg;
extern Game *gGame;
extern Ui *gUi;
extern s32 D_800958B4[];
extern MapCfg D_8007F2C4[];
extern f32 D_8007F624[];
extern u8 D_800958A1[];
void Ui_RemoveElement(void *, s32);
void func_8001FBA8(Vec3f *);

s32 func_8000E178(Marker *m) {
    Car *car;
    Widget *w = NULL;
    UiNode *node;
    MapCfg *cfg;
    f32 sx, sy;
    f32 add;
    Vec3f v;
    s8 i;

    car = NULL;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        if (gGame->x1100->x74[i]->x48 == m) {
            car = gGame->x1100->x74[i];
            break;
        }
    }
    if (car->xD) {
        Ui_RemoveElement(gUi, m->x0);
        return 1;
    }
    for (node = gUi->x8B54; node != NULL; node = node->x4) {
        if (node->x0->x0 == 0xE) {
            w = node->x0;
            break;
        }
    }
    cfg = &D_8007F2C4[D_800958B4[0]];
    sx = cfg->x18 / cfg->x8;
    sy = cfg->x1C / cfg->xC;
    func_8001FBA8(&v);
    m->x18 = (v.x + cfg->x14) * sx + w->x80;
    if (D_800958A1[0]) {
        m->x18 = (w->x80 + w->x88) - (m->x18 - w->x80);
        add = D_8007F624[D_800958B4[0]];
    } else {
        add = cfg->x20;
    }
    m->x18 += add;
    m->x1C = (v.z - cfg->x10) * sy + w->x84 + cfg->x24;
    if (m->x18 < w->x80) m->x18 = w->x80;
    if (m->x18 > w->x80 + w->x88) m->x18 = w->x80 + w->x88;
    if (m->x1C < w->x84) m->x1C = w->x84;
    if (m->x1C > w->x84 + w->x8C) m->x1C = w->x84 + w->x8C;
    return 0;
}
