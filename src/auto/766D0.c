#include "common.h"
#include "m2c.h"

typedef char *va_list;
#define _VA_ALIGN(p, a) (((unsigned int)(((char *)p) + ((a) > 4 ? (a) : 4) - 1)) & -((a) > 4 ? (a) : 4))
#define va_arg(list, mode) ((mode *)(list = (char *)((((int)list + (__alignof__(mode) <= 4 ? 3 : 7)) & (__alignof__(mode) <= 4 ? -4 : -8)) + sizeof(mode))))[-1]

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
    unsigned char qual;
} _Pft;

#define FLAGS_SPACE 1
#define FLAGS_PLUS 2
#define FLAGS_MINUS 4
#define FLAGS_HASH 8
#define FLAGS_ZERO 16
#define LDSIGN(x) (((unsigned short *)&(x))[0] & 0x8000)

void _Litob(_Pft *, unsigned char);
void _Ldtob(_Pft *, unsigned char);
u32 strlen(const char *);

void _Putfld(_Pft *px, va_list *pap, unsigned char code, char *ac) {
    px->n0 = px->nz0 = px->n1 = px->nz1 = px->n2 = px->nz2 = 0;
    switch (code) {
        case 'c':
            ac[px->n0++] = va_arg(*pap, int);
            break;
        case 'd':
        case 'i':
            if (px->qual == 'l') {
                px->v.ll = va_arg(*pap, int);
            } else if (px->qual == 'L') {
                px->v.ll = va_arg(*pap, long long);
            } else {
                px->v.ll = va_arg(*pap, int);
            }
            if (px->qual == 'h') {
                px->v.ll = (s16)px->v.ll;
            }
            if (px->v.ll < 0) {
                ac[px->n0++] = '-';
            } else if (px->flags & FLAGS_PLUS) {
                ac[px->n0++] = '+';
            } else if (px->flags & FLAGS_SPACE) {
                ac[px->n0++] = ' ';
            }
            px->s = (unsigned char *)&ac[px->n0];
            _Litob(px, code);
            break;
        case 'x':
        case 'X':
        case 'u':
        case 'o':
            if (px->qual == 'l') {
                px->v.ll = va_arg(*pap, int);
            } else if (px->qual == 'L') {
                px->v.ll = va_arg(*pap, long long);
            } else {
                px->v.ll = va_arg(*pap, int);
            }
            if (px->qual == 'h') {
                px->v.ll = (u16)px->v.ll;
            } else if (px->qual == 0) {
                px->v.ll = (unsigned int)px->v.ll;
            }
            if (px->flags & FLAGS_HASH) {
                ac[px->n0++] = '0';
                if (code == 'x' || code == 'X') {
                    ac[px->n0++] = code;
                }
            }
            px->s = (unsigned char *)&ac[px->n0];
            _Litob(px, code);
            break;
        case 'e':
        case 'f':
        case 'g':
        case 'E':
        case 'G':
            px->v.ld = px->qual == 'L' ? va_arg(*pap, double) : va_arg(*pap, double);
            if (LDSIGN(px->v.ld)) {
                ac[px->n0++] = '-';
            } else if (px->flags & FLAGS_PLUS) {
                ac[px->n0++] = '+';
            } else if (px->flags & FLAGS_SPACE) {
                ac[px->n0++] = ' ';
            }
            px->s = (unsigned char *)&ac[px->n0];
            _Ldtob(px, code);
            break;
        case 'n':
            if (px->qual == 'h') {
                *(va_arg(*pap, u16 *)) = px->nchar;
            } else if (px->qual == 'l') {
                *va_arg(*pap, unsigned int *) = px->nchar;
            } else if (px->qual == 'L') {
                *va_arg(*pap, u64 *) = px->nchar;
            } else {
                *va_arg(*pap, unsigned int *) = px->nchar;
            }
            break;
        case 'p':
            px->v.ll = (long)va_arg(*pap, void *);
            px->s = (unsigned char *)&ac[px->n0];
            _Litob(px, 'x');
            break;
        case 's':
            px->s = va_arg(*pap, unsigned char *);
            px->n1 = strlen((char *)px->s);
            if (px->prec >= 0 && px->n1 > px->prec) {
                px->n1 = px->prec;
            }
            break;
        case '%':
            ac[px->n0++] = '%';
            break;
        default:
            ac[px->n0++] = code;
            break;
    }
}
