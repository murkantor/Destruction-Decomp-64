#include "common.h"
#include "m2c.h"

typedef struct {
    s8 x0;
    s8 x1;
    s8 x2;
    s8 x3;
    s32 x4;
} Time;
extern char D_800B8270[];
extern char D_800B827C[];
int sprintf(char *, const char *, ...);

void Time_Format(Time t, char *buf, s32 len) {
    if (len == 8) {
        sprintf(buf, D_800B8270, t.x1, t.x2, t.x4);
    } else {
        sprintf(buf, D_800B827C, t.x1, t.x2, t.x4);
        if (t.x1 < 10) {
            buf[len - 9] = '0';
        }
    }
    if (t.x2 < 10) {
        buf[len - 6] = '0';
    }
    if (t.x4 < 100 && ((t.x4 >= 10 && t.x4 < 100) || t.x4 < 10)) {
        buf[len - 3] = '0';
    }
    buf[len - 1] = 0;
}
