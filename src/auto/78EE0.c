#include "common.h"
#include "m2c.h"

typedef double ldouble;
typedef struct {
    long quot;
    long rem;
} ldiv_t;
typedef struct {
    union {
        s64 ll;
        ldouble ld;
    } v;
    u8 *s;
    s32 n0;
    s32 nz0;
    s32 n1;
    s32 nz1;
    s32 n2;
    s32 nz2;
    s32 prec;
    s32 width;
    u32 nchar;
    u32 flags;
    u8 qual;
} _Pft;

extern const double D_800BC6D0[];
extern const double D_800BC720;
extern const double D_800BC728;
extern char D_800BC718[];
extern char D_800BC71C[];
s16 _Ldunscale(s16 *pex, _Pft *px);
void func_800787B8(_Pft *px, u8 code, u8 *p, s16 nsig, s16 xexp);
ldiv_t ldiv(long num, long denom);
void *memcpy(void *, const void *, unsigned int);

#define ALIGN(s, align) (((unsigned int)(s) + ((align) - 1)) & ~((align) - 1))

void _Ldtob(_Pft *px, u8 code)
{
    u8 buff[0x20];
    u8 *p;
    ldouble ldval;
    s16 err;
    s16 nsig;
    s16 xexp;

    p = buff;
    ldval = px->v.ld;

    if (px->prec < 0) {
        px->prec = 6;
    } else if (px->prec == 0 && (code == 'g' || code == 'G')) {
        px->prec = 1;
    }

    err = _Ldunscale(&xexp, px);
    if (err > 0) {
        memcpy(px->s, err == 2 ? D_800BC718 : D_800BC71C, px->n1 = 3);
        return;
    }
    if (err == 0) {
        nsig = 0;
        xexp = 0;
    } else {
        {
            int i;
            int n;
            if (ldval < 0) {
                ldval = -ldval;
            }
            if ((xexp = xexp * 30103 / 100000 - 4) < 0) {
                n = ALIGN(-xexp, 4), xexp = -n;
                for (i = 0; n > 0; n >>= 1, i++) {
                    if ((n & 1) != 0) {
                        ldval *= D_800BC6D0[i];
                    }
                }
            } else if (xexp > 0) {
                ldouble factor = D_800BC720;
                xexp &= ~3;
                for (n = xexp, i = 0; n > 0; n >>= 1, i++) {
                    if ((n & 1) != 0) {
                        factor *= D_800BC6D0[i];
                    }
                }
                ldval /= factor;
            }
        }
        {
            int gen = px->prec + ((code == 'f') ? xexp + 10 : 6);
            if (gen > 0x13) {
                gen = 0x13;
            }
            *p++ = '0';
            while (gen > 0 && 0 < ldval) {
                int j;
                long lo = ldval;
                if ((gen -= 8) > 0) {
                    ldval = (ldval - lo) * D_800BC728;
                }
                for (p += 8, j = 8; lo > 0 && --j >= 0;) {
                    ldiv_t qr;
                    qr = ldiv(lo, 10);
                    *--p = qr.rem + '0';
                    lo = qr.quot;
                }
                while (--j >= 0) {
                    *--p = '0';
                }
                p += 8;
            }

            gen = p - &buff[1];
            p = &buff[1];
            xexp += 7;
            for (; *p == '0'; p++) {
                --gen, --xexp;
            }

            nsig = px->prec + ((code == 'f') ? xexp + 1 : ((code == 'e' || code == 'E') ? 1 : 0));
            if (gen < nsig) {
                nsig = gen;
            }
            if (nsig > 0) {
                u8 drop = nsig < gen && '5' <= p[nsig] ? '9' : '0';
                int n2;

                for (n2 = nsig; p[--n2] == drop;) {
                    --nsig;
                }
                if (drop == '9') {
                    ++p[n2];
                }
                if (n2 < 0) {
                    --p, ++nsig, ++xexp;
                }
            }
        }
    }
    func_800787B8(px, code, p, nsig, xexp);
}
