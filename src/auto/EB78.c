#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { u8 pad[0x80]; u64 x80; } CarData;
typedef struct { CarData *x0; u8 pad[0x64]; } Car;
extern Car D_800958F0[];
extern char D_8007F74C[][9];
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
char *strcpy(char *, const char *);

s32 Hud_FormatLapTime(s32 unused, s8 idx) {
    char buf[16];
    if (D_800958F0[idx].x0->x80 == 0) {
        strcpy(D_8007F74C[idx], "-:--:--");
        return 0;
    }
    Time_Format(Time_Split(D_800958F0[idx].x0->x80), buf, 9);
    strcpy(D_8007F74C[idx], buf);
    return 0;
}
