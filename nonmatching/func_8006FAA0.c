typedef struct {
    s32 status;
} OSPfs;
s32 __osGetId(OSPfs *);

s32 func_8006FAA0(OSPfs *pfs) {
    s32 ret;

    if ((pfs->status & 5) == 0) {
        return 5;
    }
    ret = __osGetId(pfs);
    if (ret != 0) {
        return ret;
    }
    pfs->status &= ~4;
    return ret;
}
