#include "common.h"
#include "m2c.h"

extern char D_8007F714[][9];
extern char D_8007F74C[][9];
extern char D_8007F828[];
extern char D_8007F834[];
extern char D_8007F83C[];
extern u8 D_8007F6C1;
char *strcpy(char *, const char *);
s32 sprintf(char *, const char *, ...);

void Hud_ResetTimeStrings(void) {
    char buf[] = "--:--:--";
    s8 i;

    for (i = 0; i < 3; i++) {
        strcpy(D_8007F714[i], buf);
        strcpy(D_8007F714[i + 3], buf);
    }
    for (i = 0; i < 4; i++) {
        strcpy(D_8007F74C[i], buf);
    }
    strcpy(D_8007F828, buf);
    sprintf(D_8007F83C, "  ");
    sprintf(D_8007F834, "  ");
    sprintf(D_8007F834 + 3, "  ");
    D_8007F6C1 = 0;
}
