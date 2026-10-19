typedef union { u16 v; struct { u8 a; u8 b; } s; } PakEnt;
typedef struct { u8 pad[0x60]; s32 x60; } UnkObj;

s32 func_8006E76C(UnkObj *o, PakEnt *tbl, s32 n, s32 *start, u8 flag, s32 *cnt, s32 *last) {
    s32 ret;
    s32 s = 1;
    s32 i;
    s32 prev;
    s32 first;

    if (flag == 0) {
        s = o->x60;
        ret = 0;
    } else {
        ret = 0;
    }
    for (i = s; i < 0x80; i++) {
        if (tbl[i].v == 3) {
            break;
        }
    }
    if (i == 0x80) {
        *start = -1;
    } else {
        *cnt = 1;
        prev = i;
        first = prev;
        for (i = prev + 1; *cnt < n && i < 0x80; i++) {
            if (tbl[i].v == 3) {
                tbl[prev].s.a = flag;
                tbl[prev].s.b = i;
                prev = i;
                (*cnt)++;
            }
        }
        *start = first;
        if (i == 0x80 && *cnt < n) {
            *last = prev;
        } else {
            tbl[prev].v = 1;
            *last = 0;
        }
    }
    return ret;
}
