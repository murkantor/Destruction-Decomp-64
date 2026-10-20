typedef struct { u8 data[0x68]; } OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsChecker(OSPfs *);

s32 Pak_Check(s32 channel) {
    if (gPakReady[channel] != 0) {
        channel = osPfsChecker(&gPakFs[channel]);
    } else {
        return -1;
    }
    return channel;
}
/* agent59: this if/else form gives the original block layout (bnez over
 * "j end; li v0,-1"), but combine folds "move v1,v0; move v0,v1" after the
 * jal away (size 0x58 vs 0x60). The old draft (channel = -1 in the then-branch)
 * keeps the copies but puts -1 in v1. Tried: ret var, ret = -1 init, ternaries,
 * switch, for/do-while(0) wrappers, goto, inline chk()/id() wrappers,
 * "if (ret) return ret; return ret;" (keeps move v1,v0 but adds a branch), -O1.
 * Next idea: a local register variable (register s32 r asm("$3")). */
