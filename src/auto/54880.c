#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x30];
    u16 shnum;
} Elf;
void Elf_ResolveSymbols(Elf *);
void Elf_Relocate(Elf *, s32);

void Elf_Link(Elf *elf, s32 base) {
    s32 i;
    char unused[1];

    for (i = 0; i < elf->shnum; i++) {
    }
    for (i = 0; i < elf->shnum; i++) {
    }
    Elf_ResolveSymbols(elf);
    Elf_Relocate(elf, base);
}
