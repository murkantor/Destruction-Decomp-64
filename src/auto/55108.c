#include "common.h"
#include "m2c.h"

typedef struct {
    u32 type;
    char *label;
    void *val;
    union {
        struct { u8 min; u8 max; u8 pad[2]; char **names; } b;
        struct { s32 min; s32 max; char **names; } w;
    } u;
} Item;
typedef struct { s32 sel; s32 count; Item items[1]; } Menu;
typedef struct { u8 pad[0x8C]; void *x8C; void *x90; } Ctx;
extern char D_800BBB44[], D_800BBB48[], D_800BBB4C[], D_800BBB54[], D_800BBB5C[], D_800BBB60[], D_800BBB64[];
extern Menu *func_80054028(void);
extern void func_80057408(void *, s32, s32, char *);
extern void func_80055F70(void *);
extern void func_80056098(void *);
extern int sprintf(char *, const char *, ...);

void DebugMenu_DrawPage(Ctx *ctx) {
    Menu *menu = func_80054028();
    Item *it;
    s32 i;
    char buf[0x40];
    s32 lx = 30;
    s32 vx = 150;
    s32 y;

    for (i = 0; i < menu->count; i++) {
        it = &menu->items[i];
        y = i * 12 + 30;
        if (i == menu->sel) {
            func_80057408(ctx, 20, y, D_800BBB44);
            func_80057408(ctx, 0x127, y, D_800BBB48);
        }
        func_80057408(ctx, lx, y, it->label);
        switch (it->type) {
        case 1:
            if (it->u.b.names != NULL) {
                s32 v = *(u8 *)it->val;
                if (v < it->u.b.min) {
                    sprintf(buf, D_800BBB4C, v);
                    func_80057408(ctx, vx, y, buf);
                } else if (it->u.b.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    func_80057408(ctx, vx, y, buf);
                } else {
                    v -= it->u.b.min;
                    func_80057408(ctx, vx, y, it->u.b.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(u8 *)it->val);
                func_80057408(ctx, vx, y, buf);
            }
            break;
        case 0:
            if (it->u.w.names != NULL) {
                s32 v = *(s32 *)it->val;
                if (v < it->u.w.min) {
                    sprintf(buf, D_800BBB4C, v);
                    func_80057408(ctx, vx, y, buf);
                } else if (it->u.w.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    func_80057408(ctx, vx, y, buf);
                } else {
                    v -= it->u.w.min;
                    func_80057408(ctx, vx, y, it->u.w.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(s32 *)it->val);
                func_80057408(ctx, vx, y, buf);
            }
            break;
        case 2:
            sprintf(buf, D_800BBB60, *(f32 *)it->val);
            func_80057408(ctx, vx, y, buf);
            break;
        case 3:
            func_80057408(ctx, vx, y, D_800BBB64);
            break;
        }
    }
    func_80055F70(ctx->x8C);
    func_80056098(ctx->x90);
}
