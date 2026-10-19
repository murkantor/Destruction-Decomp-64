#include "common.h"
#include "m2c.h"

struct huft {
    u8 e;
    u8 b;
    union {
        u16 n;
        struct huft *t;
    } v;
};

extern u32 D_800BCA2C;
void *memset(void *, s32, u32);
void *Mem_AllocAlt(u32 size);
s32 huft_free(struct huft *t);

s32 huft_build(u32 *b, u32 n, u32 s, u16 *d, u16 *e, struct huft **t, s32 *m) {
    u32 a;
    u32 c[17];
    u32 el;
    u32 f;
    s32 g;
    s32 h;
    register u32 i;
    register u32 j;
    register s32 k;
    s32 lx[17];
    s32 *l = lx + 1;
    register u32 *p;
    register struct huft *q;
    struct huft r;
    struct huft *u[16];
    u32 v[288];
    register s32 w;
    u32 x[17];
    u32 *xp;
    s32 y;
    u32 z;

    el = n > 256 ? b[256] : 16;
    memset(c, 0, sizeof(c));
    p = b;
    i = n;
    do {
        c[*p]++;
        p++;
    } while (--i);
    if (c[0] == n) {
        *t = NULL;
        *m = 0;
        return 0;
    }

    for (j = 1; j <= 16; j++)
        if (c[j])
            break;
    k = j;
    if ((u32)*m < j)
        *m = j;
    for (i = 16; i; i--)
        if (c[i])
            break;
    g = i;
    if ((u32)*m > i)
        *m = i;

    for (y = 1 << j; j < i; j++, y <<= 1)
        if ((y -= c[j]) < 0)
            return 2;
    if ((y -= c[i]) < 0)
        return 2;
    c[i] += y;

    x[1] = j = 0;
    p = c + 1;
    xp = x + 2;
    while (--i) {
        *xp++ = (j += *p++);
    }

    memset(v, 0, sizeof(v));
    p = b;
    i = 0;
    do {
        if ((j = *p++) != 0)
            v[x[j]++] = i;
    } while (++i < n);
    n = x[g];

    x[0] = i = 0;
    p = v;
    h = -1;
    w = l[-1] = 0;
    u[0] = NULL;
    q = NULL;
    z = 0;

    for (; k <= g; k++) {
        a = c[k];
        while (--a != (u32)-1) {
            while (k > w + l[h]) {
                w += l[h++];
                z = (z = g - w) > (u32)*m ? *m : z;
                if ((f = 1 << (j = k - w)) > (a + 1)) {
                    f -= (a + 1);
                    xp = c + k;
                    while (++j < z) {
                        if ((f <<= 1) <= *++xp)
                            break;
                        f -= *xp;
                    }
                }
                if ((u32)w + j > el && (u32)w < el)
                    j = el - w;
                z = 1 << j;
                l[h] = j;

                if ((q = (struct huft *)Mem_AllocAlt((z + 1) * sizeof(struct huft))) == NULL) {
                    if (h)
                        huft_free(u[0]);
                    return 3;
                }
                D_800BCA2C += z + 1;
                *t = q + 1;
                *(t = &(q->v.t)) = NULL;
                u[h] = ++q;

                if (h) {
                    x[h] = i;
                    r.b = (u8)l[h - 1];
                    r.e = (u8)(16 + j);
                    r.v.t = q;
                    j = (i & ((1 << w) - 1)) >> (w - l[h - 1]);
                    u[h - 1][j] = r;
                }
            }

            r.b = (u8)(k - w);
            if (p >= v + n)
                r.e = 99;
            else if (*p < s) {
                r.e = (u8)(*p < 256 ? 16 : 15);
                r.v.n = *p++;
            } else {
                r.e = (u8)e[*p - s];
                r.v.n = d[*p++ - s];
            }

            f = 1 << (k - w);
            for (j = i >> w; j < z; j += f)
                q[j] = r;

            for (j = 1 << (k - 1); i & j; j >>= 1)
                i ^= j;
            i ^= j;

            while ((i & ((1 << w) - 1)) != x[h])
                w -= l[--h];
        }
    }

    *m = l[0];
    return y != 0 && g != 1;
}
