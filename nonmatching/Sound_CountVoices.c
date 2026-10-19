extern s32 gNumVoices;
extern u8 *gVoices;

s32 Sound_CountVoices(s32 key) {
    if (key != 0) {
        s32 count = 0;
        u8 *p = gVoices;
        s32 i;

        for (i = 0; i < gNumVoices; i++) {
            count += *(s32 *)(p + 0x40) == key;
            p += 0x120;
        }
        return count;
    }
    return 0;
}
