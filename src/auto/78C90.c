#include "common.h"
#include "m2c.h"

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
typedef struct {
    long long quot;
    long long rem;
} lldiv_t;
extern char ldigs[];
extern char udigs[];
lldiv_t lldiv(long long, long long);
void *memcpy(void *, const void *, u32);

void _Litob(_Pft *args, u8 type) {
    char buff[0x18];
    const char *digs;
    s32 base;
    s32 i;
    unsigned long long ullval;

    digs = (type == 'X') ? udigs : ldigs;
    base = (type == 'o') ? 8 : ((type != 'x' && type != 'X') ? 10 : 16);
    i = 0x18;
    ullval = args->v.ll;
    if ((type == 'd' || type == 'i') && args->v.ll < 0) {
        ullval = -ullval;
    }
    if (ullval != 0 || args->prec != 0) {
        buff[--i] = digs[ullval % base];
    }
    args->v.ll = ullval / base;
    while (args->v.ll > 0 && i > 0) {
        lldiv_t qr = lldiv(args->v.ll, base);

        args->v.ll = qr.quot;
        buff[--i] = digs[qr.rem];
    }
    args->n1 = 0x18 - i;
    memcpy(args->s, buff + i, args->n1);
    if (args->n1 < args->prec) {
        args->nz0 = args->prec - args->n1;
    }
    if (args->prec < 0 && (args->flags & (0x14)) == 0x10) {
        i = args->width - args->n0 - args->nz0 - args->n1;
        if (i > 0) {
            args->nz0 += i;
        }
    }
}
