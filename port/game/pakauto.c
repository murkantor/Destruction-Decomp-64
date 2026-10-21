/*
 * No Controller Pak screens ([saves] pak_screens = 0, the default): the PC's
 * paks are files and load or save at once, so the game's pak screens (checking
 * the pak, loading, saving with a "SUCCESSFUL" wait, no pak, no note, create a
 * note, overwrite, pak full, damaged, new pak, insert paks, the note manager)
 * are skipped. Every request for one (Game_SetNextMode) runs here instead: the
 * load or save the game has pending runs now, a missing note is made as the
 * "create note" choice makes it (Pak_SaveGame), and the game goes on where its
 * successful load or save goes (PakMenu_AutoSaveTick: the title, or the ending
 * after the last cup). Failures are logged, not shown.
 *
 * The modes (gGame->x10FC):
 *   3, 4    the note manager (START held at boot, or a full pak), and its delete
 *   5       checking the pak, then 9 (load), 0xB (save) or an error screen
 *   6       no pak          7  no note (create one?)    8  go on without saving?
 *   9, 0xB  loading, saving (PakMenu_AutoSaveTick)        0xA  overwrite?
 *   0xC     pak full        0xD  damaged                 0xE  new pak
 *   0xF     the paks checked before a race (PakMenu_CheckPaks), then 0x10:
 *           insert paks (multiplayer)
 */
#include "common.h"

extern u8 D_8009734C;  /* a load is pending (else a save, with D_8009734D) */
extern u8 D_8009734D;
extern u8 D_800972F0;  /* the last cup is won: the ending comes next */
extern s32 D_80097D50; /* the save note's number, -1 for none */

int Port_PakScreens(void);
void Port_Log(const char *fmt, ...);
u8 Pak_Open(s32 *);
void Pak_LoadSave(s32 *);
void Pak_WriteSave(s32 *);
void Pak_SaveGame(s32 *);
void func_80063380(s32);

/* Saves the game, into a new note if there is none */
static s32 save(void) {
    s32 e;

    if (!Pak_Open(&e) || (e != 0 && e != 5)) {
        return e;
    }
    if (e == 5 || D_80097D50 == -1) {
        Pak_SaveGame(&e); /* makes the note (Pak_AllocateFile) and writes it */
    } else {
        Pak_WriteSave(&e);
    }
    return e;
}

/* Loads the save, making the note if there is none (or the one there was not
   the game's: Pak_LoadSave deletes it) */
static s32 load(void) {
    s32 e;

    if (!Pak_Open(&e) || (e != 0 && e != 5)) {
        return e;
    }
    if (e == 0) {
        Pak_LoadSave(&e);
    }
    if (e == 5) {
        Pak_SaveGame(&e);
    }
    return e;
}

/* Game_SetNextMode's mode, with the pak screens taken out */
s16 Port_PakAuto(s16 mode) {
    static int busy;
    s32 e = 0;

    if (Port_PakScreens() || busy) {
        return mode;
    }
    if (mode == 0xF || mode == 0x10) {
        func_80063380(2); /* as PakMenu_CheckPaks with the paks there, or
                             PakMenu_InsertPaksSelect: on to the course intro */
        return 0x1A;
    }
    if (mode < 3 || mode > 0xE) {
        return mode;
    }
    busy = 1; /* the pak calls may route errors here again (Pak_HandleError) */
    if (D_8009734C) {
        e = load();
        Port_Log("pak: loaded the save (%d)", (int)e);
    } else if (D_8009734D) {
        e = save();
        Port_Log("pak: saved (%d)", (int)e);
    } else if (mode == 7) {
        e = save(); /* no note: made, as the "create note" choice does */
        Port_Log("pak: made the save note (%d)", (int)e);
    } else {
        Port_Log("pak: screen %d skipped", (int)mode);
    }
    D_8009734C = 0;
    D_8009734D = 0;
    busy = 0;
    if (D_800972F0) {
        return 0x2E;
    }
    func_80063380(2);
    return 0x19;
}
