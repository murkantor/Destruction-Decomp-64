typedef struct { u8 data[0x68]; } OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsChecker(OSPfs *);

s32 Pak_Check(s32 channel) {
    if (gPakReady[channel] == 0) {
        channel = -1;
    } else {
        channel = osPfsChecker(&gPakFs[channel]);
    }
    return channel;
}
