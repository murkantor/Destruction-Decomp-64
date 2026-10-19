extern s32 gNumVoices;
extern u8 *gVoices;

s32 Sound_SetVolume(s32 key, s16 value) {
    if (key != 0) {
        s32 count = 0;
        u8 *p = gVoices;
        s32 i;

        for (i = 0; i < gNumVoices; i++) {
            if (*(s32 *)(p + 0x40) == key) {
                *(s16 *)(p + 0x96) = value;
                count++;
            }
            p += 0x120;
        }
        return count;
    }
    return 0;
}
