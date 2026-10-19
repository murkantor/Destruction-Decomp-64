typedef struct {
    u8 data[0x68];
} OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsChecker(OSPfs *);

s32 Pak_Check(s32 channel) {
    if (gPakReady[channel] != 0) {
        s32 ret = osPfsChecker(&gPakFs[channel]);

        return ret;
    }
    return -1;
}
