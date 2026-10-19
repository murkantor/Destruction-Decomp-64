#include "common.h"
#include "m2c.h"

extern u8 _ctype[];
#define isspace(c) (_ctype[(u8)(c)] & 1)
#define isdigit(c) (_ctype[(u8)(c)] & 8)

int atoi(const char *s)
{
    int n = 0;
    char sign;
    char c;

    while (isspace(*s))
        s++;
    sign = *s;
    if (sign == '-') goto skip; if (sign == '+') skip: s++;
    while (isdigit(c = *s++))
        n = n * 10 - ('0' - c);
    if (sign == '-')
        n = -n;
    return n;
}
