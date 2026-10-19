typedef struct {
    union {
        long long ll;
        double ld;
    } v;
    unsigned char *s;
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
    char qual;
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

#define isdigit(x) ((unsigned)((x) - '0') < 10)
#define va_arg(ap, type) (((type *)(ap = (char *)(((int)(ap) + 3) & -4) + 4))[-1])

#define _PROUT(fmt, _size) \
    if (_size > 0) { \
        arg = (void *)prout(arg, fmt, _size); \
        if (arg != 0) { \
            x.nchar += _size; \
        } else { \
            return x.nchar; \
        } \
    }

#define _PAD(s, m) \
    if (0 < m) { \
        int i, j; \
        for (j = m; 0 < j; j -= i) { \
            if ((unsigned)j < sizeof(s)) { \
                i = j; \
            } else { \
                i = sizeof(s) - 1; \
            } \
            _PROUT(s, i); \
        } \
    }

int _Printf(outfun prout, char *arg, const u8 *fmt, va_list args) {
    _Pft x;

    x.nchar = 0;
    while (1) {
        const u8 *s;
        u8 c;
        const char *t;
        char ac[32];

        s = fmt;
        for (c = *s; c != 0 && c != '%';) {
            c = *++s;
        }
        _PROUT(fmt, s - fmt);
        if (c == 0) {
            return x.nchar;
        }
        fmt = ++s;
        x.flags = 0;
        for (; (t = strchr(D_800BC530, *s)) != NULL; s++) {
            x.flags |= D_800BC538[t - D_800BC530];
        }
        if (*s == '*') {
            x.width = va_arg(args, int);
            if (x.width < 0) {
                x.width = -x.width;
                x.flags |= 4;
            }
            s++;
        } else {
            for (x.width = 0; isdigit(*s); s++) {
                if (x.width < 999) {
                    x.width = *s + x.width * 10 - '0';
                }
            }
        }
        if (*s != '.') {
            x.prec = -1;
        } else if (*++s == '*') {
            x.prec = va_arg(args, int);
            ++s;
        } else {
            for (x.prec = 0; isdigit(*s); s++) {
                if (x.prec < 999) {
                    x.prec = *s + x.prec * 10 - '0';
                }
            }
        }
        x.qual = strchr(D_800BC550, *s) ? *s++ : '\0';
        if (x.qual == 'l' && *s == 'l') {
            x.qual = 'L';
            ++s;
        }
        _Putfld(&x, &args, *s, ac);
        x.width -= x.n0 + x.nz0 + x.n1 + x.nz1 + x.n2 + x.nz2;
        if (!(x.flags & 4)) {
            _PAD(spaces, x.width);
        }
        _PROUT(ac, x.n0);
        _PAD(zeroes, x.nz0);
        _PROUT(x.s, x.n1);
        _PAD(zeroes, x.nz1);
        _PROUT((char *)(&x.s[x.n1]), x.n2);
        _PAD(zeroes, x.nz2);
        if (x.flags & 4) {
            _PAD(spaces, x.width);
        }
        fmt = s + 1;
    }
}
