typedef struct {
    s32 x0;
    s32 start;
    s32 exit;
    s32 length;
} Shortcut;
typedef struct {
    u8 pad[0x38];
    s32 numNodes;
    Shortcut *shortcuts;
} RaceState;
typedef struct {
    u8 pad[0x1100];
    RaceState *race;
} GameState;
typedef struct {
    u8 pad[0x28];
    s32 node;
    u8 pad2C[4];
    s32 x30;
    u8 pad34[0xC];
    u8 onShortcut;
    u8 pad41[3];
    s32 shortcut;
} Racer;
extern GameState *gGame;

s32 func_80020E58(Racer *c, s32 node) {
    RaceState *r = gGame->race;
    s32 ret = node;
    Shortcut *s;

    if (c->onShortcut != 0 && c->x30 == 1) {
        s = c->shortcut + r->shortcuts;
        if (node >= s->start + s->length) {
            c->onShortcut = 0;
            c->shortcut = -1;
            c->node = s->exit;
            ret = c->node;
        } else if (node < s->start) {
            ret = s->start;
        }
    } else {
        if (node < 0) {
            ret = r->numNodes + node;
        }
        if (node >= r->numNodes) {
            ret = node - r->numNodes;
        }
    }
    return ret;
}
