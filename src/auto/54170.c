#include "common.h"
#include "m2c.h"

typedef struct {
    u8 e_ident[16];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u32 e_entry;
    u32 e_phoff;
    u32 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    u32 sh_name;
    u32 sh_type;
    u32 sh_flags;
    u32 sh_addr;
    u32 sh_offset;
    u32 sh_size;
    u32 sh_link;
    u32 sh_info;
    u32 sh_addralign;
    u32 sh_entsize;
} Elf32_Shdr;

typedef struct {
    u32 st_name;
    u32 st_value;
    u32 st_size;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
} Elf32_Sym;

extern char D_800BBA64[];
extern char D_800BBA6C[];
int strcmp(const char *, const char *);

#define SHDR(e, i) ((Elf32_Shdr *)((u8 *)(e) + (e)->e_shoff + (e)->e_shentsize * (i)))

static inline Elf32_Shdr *Elf_FindSection(Elf32_Ehdr *elf, char *secname) {
    Elf32_Shdr *sh;
    Elf32_Shdr *cur;
    s32 i;

    sh = (Elf32_Shdr *)((u8 *)elf + elf->e_shoff);
    for (i = 0; i < elf->e_shnum; i++) {
        cur = SHDR(elf, i);
        if (strcmp((char *)elf + SHDR(elf, elf->e_shstrndx)->sh_offset + cur->sh_name, secname) == 0) {
            return sh;
        }
        sh = (Elf32_Shdr *)((u8 *)sh + elf->e_shentsize);
    }
    return NULL;
}

void *Elf_FindSymbol(void *data, char *name) {
    Elf32_Ehdr *elf = data;
    Elf32_Shdr *symtab;
    Elf32_Shdr *strtab;
    Elf32_Sym *sym;
    s32 i;

    symtab = Elf_FindSection(elf, D_800BBA64);
    strtab = Elf_FindSection(elf, D_800BBA6C);
    for (i = 0; i < (s32)(symtab->sh_size / symtab->sh_entsize); i++) {
        sym = (Elf32_Sym *)((u8 *)elf + symtab->sh_offset + symtab->sh_entsize * i);
        if (strcmp(name, (char *)elf + strtab->sh_offset + sym->st_name) == 0) {
            u32 k = sym->st_shndx;
            return (u8 *)elf + SHDR(elf, k)->sh_offset + sym->st_value;
        }
    }
    return NULL;
}
