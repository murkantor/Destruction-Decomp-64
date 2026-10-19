#include "common.h"
#include "m2c.h"

typedef char *va_list;
int _Printf(void *(*)(void *, const char *, u32), void *, const char *, va_list);
void *proutSprintf(void *, const char *, u32);

int sprintf(char *s, const char *fmt, ...) {
    int ans;
    va_list ap;

    ap = (va_list)__builtin_next_arg(fmt);
    ans = _Printf(proutSprintf, s, fmt, ap);
    if (ans >= 0) {
        s[ans] = 0;
    }
    return ans;
}
