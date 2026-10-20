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
extern Menu *DebugMenu_GetCurrent(void);
extern void Font_BlitString(void *, s32, s32, char *);
extern void Vi_ApplyConfig(void *);
extern void Vi_SwapBuffer(void *);
extern int sprintf(char *, const char *, ...);

void DebugMenu_DrawPage(Ctx *ctx) {
    Menu *menu = DebugMenu_GetCurrent();
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
            Font_BlitString(ctx, 20, y, D_800BBB44);
            Font_BlitString(ctx, 0x127, y, D_800BBB48);
        }
        Font_BlitString(ctx, lx, y, it->label);
        switch (it->type) {
        case 1:
            if (it->u.b.names != NULL) {
                s32 v = *(u8 *)it->val;
                if (v < it->u.b.min) {
                    sprintf(buf, D_800BBB4C, v);
                    Font_BlitString(ctx, vx, y, buf);
                } else if (it->u.b.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    Font_BlitString(ctx, vx, y, buf);
                } else {
                    v -= it->u.b.min;
                    Font_BlitString(ctx, vx, y, it->u.b.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(u8 *)it->val);
                Font_BlitString(ctx, vx, y, buf);
            }
            break;
        case 0:
            if (it->u.w.names != NULL) {
                s32 v = *(s32 *)it->val;
                if (v < it->u.w.min) {
                    sprintf(buf, D_800BBB4C, v);
                    Font_BlitString(ctx, vx, y, buf);
                } else if (it->u.w.max < v) {
                    sprintf(buf, D_800BBB54, v);
                    Font_BlitString(ctx, vx, y, buf);
                } else {
                    v -= it->u.w.min;
                    Font_BlitString(ctx, vx, y, it->u.w.names[v]);
                }
            } else {
                sprintf(buf, D_800BBB5C, *(s32 *)it->val);
                Font_BlitString(ctx, vx, y, buf);
            }
            break;
        case 2:
            sprintf(buf, D_800BBB60, *(f32 *)it->val);
            Font_BlitString(ctx, vx, y, buf);
            break;
        case 3:
            Font_BlitString(ctx, vx, y, D_800BBB64);
            break;
        }
    }
    Vi_ApplyConfig(ctx->x8C);
    Vi_SwapBuffer(ctx->x90);
}
