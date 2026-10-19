extern u8 D_800D31C0[];
extern u8 D_80131F90[];
extern u8 D_80134280[];
extern u8 D_BD4A0[];
extern u8 D_BF790[];
extern u8 gRomData1End[];
extern u8 gResArchiveRom[];
void func_80063300(void *);
void func_80058848(void *, void *, s32);
void func_8005C830(void);
void func_800631E0(void *);

void func_80000470(void) {
    func_80063300(D_800D31C0);
    func_80058848(D_80131F90, D_BD4A0, gRomData1End - D_BD4A0);
    func_8005C830();
    func_80058848(D_80134280, D_BF790, gResArchiveRom - D_BF790);
    func_800631E0(D_800D31C0);
}
