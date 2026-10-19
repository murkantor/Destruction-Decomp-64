#include "common.h"
#include "m2c.h"

extern s32 D_800B04A8;
extern s32 D_800B04A4;
extern u32 D_80125C90;
extern char D_800BBA88[], D_800BBAA8[], D_800BBAC8[], D_800BBAD4[], D_800BBAE0[];
void func_80056408(void *);
void Font_BlitString(void *, s32, s32, char *);
s32 sprintf(char *, const char *, ...);
void Pad_Read(void);
void func_80053EA8(void);

void Debug_AssertScreen(char *expr, char *file, s32 line) {
    u8 ctx[0xC0];
    char buf[0x80];

    if (D_800B04A8 != 0) {
        func_80056408(ctx);
        Font_BlitString(ctx, 30, 60, D_800BBA88);
        Font_BlitString(ctx, 30, 70, D_800BBAA8);
        sprintf(buf, D_800BBAC8, file);
        Font_BlitString(ctx, 30, 80, buf);
        sprintf(buf, D_800BBAD4, line);
        Font_BlitString(ctx, 30, 90, buf);
        Font_BlitString(ctx, 30, 100, D_800BBAA8);
        Font_BlitString(ctx, 30, 120, D_800BBAE0);
        do {
            Pad_Read();
        } while (!(D_80125C90 & 0x1000));
    } else {
        switch (D_800B04A4) {
            case 0:
                break;
            case 1:
                func_80053EA8();
                break;
        }
    }
}
