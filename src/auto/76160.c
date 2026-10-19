#include "common.h"
#include "m2c.h"

typedef struct {
    union {
        long long ll;
        double ld;
    } v;
    char *s;
    int n0;
    int nz0;
    int n1;
    int nz1;
    int n2;
    int nz2;
    int prec;
    int width;
    u32 nchar;
    unsigned int flags;
    u8 qual;
} _Pft;
typedef char *va_list;
typedef void *(*outfun)(void *, const char *, u32);
extern char spaces[33];
extern char zeroes[33];
extern const char D_800BC530[];
extern const int D_800BC538[];
extern const char D_800BC550[];
const char *strchr(const char *, int);
void _Putfld(_Pft *, va_list *, u8, char *);

#define isdigit(x) ((x >= '0' && x <= '9'))
#define va_arg(ap, type) (*((type *)(ap = (char *)(((int)(ap) + 3) & -4) + 4) - 1))
#define ATOI(dst, src)                  \
    for (dst = 0; isdigit(*src); ++src) \
    {                                   \
        if (dst < 999)                  \
            dst = dst * 10 + *src - '0';\
    }
#define MAX_PAD ((sizeof(spaces) - 1))
#define PAD(s, n)                                     \
    if (0 < (n))                                      \
    {                                                 \
        int i, j = (n);                               \
        for (; 0 < j; j -= i)                         \
        {                                             \
            i = MAX_PAD < (unsigned int)j ? (int)MAX_PAD : j; \
            PUT(s, i);                                \
        }                                             \
    }
#define PUT(s, n)                                     \
    if (0 < (n))                                      \
    {                                                 \
        if ((arg = (*pfn)(arg, s, n)) != NULL)        \
            x.nchar += (n);                           \
        else                                          \
            return x.nchar;                           \
    }
int _Printf(outfun pfn, char *arg, const u8 *fmt, va_list ap)
{
    _Pft x;
    x.nchar = 0;
    for (;;)
    {
        const u8 *s; u8 c;
        const char *t;
        char ac[32];
        s = fmt;
        for (c = *s; c != 0 && c != '%';)
        {
            c = *++s;
        }
        PUT(fmt, s - fmt);
        if (c == 0)
        {
            return x.nchar;
        }
        fmt = ++s;
        for (x.flags = 0; (t = strchr(D_800BC530, *s)) != NULL; s++)
        {
            x.flags |= D_800BC538[t - D_800BC530];
        }
        if (*s == '*')
        {
            x.width = va_arg(ap, int);
            if (x.width < 0)
            {
                x.width = -x.width;
                x.flags |= 4;
            }
            s++;
        }
        else
            ATOI(x.width, s);
        if (*s != '.')
        {
            x.prec = -1;
        }
        else if (*++s == '*')
        {
            x.prec = va_arg(ap, int);
            ++s;
        }
        else
            ATOI(x.prec, s);
        x.qual = strchr(D_800BC550, *s) ? *s++ : '\0';
        if (x.qual == 'l' && *s == 'l')
        {
            x.qual = 'L';
            ++s;
        }
        _Putfld(&x, &ap, *s, ac);
        x.width -= x.n0 + x.nz0 + x.n1 + x.nz1 + x.n2 + x.nz2;
        if (!(x.flags & 4))
        {
            PAD(spaces, x.width);
        }
        PUT(ac, x.n0);
        PAD(zeroes, x.nz0);
        PUT(x.s, x.n1);
        PAD(zeroes, x.nz1);
        PUT(x.s + x.n1, x.n2)
        PAD(zeroes, x.nz2);
        if (x.flags & 4)
        {
            PAD(spaces, x.width);
        }
        fmt = s + 1;
    }
    return 0;
}
