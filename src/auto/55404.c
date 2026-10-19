#include "common.h"
#include "m2c.h"

typedef struct {
    u32 type;
    char *label;
    void *val;
    union {
        struct {
            u8 min;
            u8 max;
            u8 pad[2];
            char **names;
        } b;
        struct {
            s32 min;
            s32 max;
            char **names;
        } w;
    } u;
} MenuItem;

typedef struct {
    s32 sel;
    s32 n;
    MenuItem items[1];
} Menu;

extern char D_800BBB44[];
extern char D_800BBB48[];
extern char D_800BBB4C[];
extern char D_800BBB54[];
extern char D_800BBB5C[];
extern char D_800BBB60[];
extern char D_800BBB64[];
Menu *func_80054028(void);
void *func_80057964(void *g, s32 x, s32 y, char *s);
s32 sprintf(char *, const char *, ...);

void *DebugMenu_Draw(void *g) {
    Menu *m = func_80054028();
    MenuItem *it;
    s32 i;
    s32 y;
    s32 v;
    s32 xl = 30;
    s32 xv = 150;
    char buf[64];

    for (i = 0; i < m->n; i++) {
        it = &m->items[i];
        y = i * 12 + 30;
        if (i == m->sel) {
            g = func_80057964(g, 20, y, D_800BBB44);
            g = func_80057964(g, 0x127, y, D_800BBB48);
        }
        g = func_80057964(g, xl, y, it->label);
        switch (it->type) {
        case 1:
            if (it->u.b.names != NULL) {
                v = *(u8 *)it->val;
                if (v < it->u.b.min) {
                    sprintf(buf, D_800BBB4C, v);
                    g = func_80057964(g, xv, y, buf);
                } else if (it->u.b.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    g = func_80057964(g, xv, y, buf);
                } else {
                    v -= it->u.b.min;
                    g = func_80057964(g, xv, y, it->u.b.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(u8 *)it->val);
                g = func_80057964(g, xv, y, buf);
            }
            break;
        case 0:
            if (it->u.w.names != NULL) {
                v = *(s32 *)it->val;
                if (v < it->u.w.min) {
                    sprintf(buf, D_800BBB4C, v);
                    g = func_80057964(g, xv, y, buf);
                } else if (it->u.w.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    g = func_80057964(g, xv, y, buf);
                } else {
                    v -= it->u.w.min;
                    g = func_80057964(g, xv, y, it->u.w.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(s32 *)it->val);
                g = func_80057964(g, xv, y, buf);
            }
            break;
        case 2:
            sprintf(buf, D_800BBB60, *(f32 *)it->val);
            g = func_80057964(g, xv, y, buf);
            break;
        case 3:
            g = func_80057964(g, xv, y, D_800BBB64);
            break;
        }
    }
    return g;
}
