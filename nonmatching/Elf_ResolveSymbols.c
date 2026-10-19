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

extern char D_800BBA64[];
extern char D_800BBA6C[];
s32 strcmp(const char *, const char *);

static inline char *SecName(Elf32_Ehdr *elf, s32 i) {
    u32 off = elf->e_shentsize * i;
    u32 stroff = elf->e_shentsize * elf->e_shstrndx;
    u8 *base = (u8 *)elf + elf->e_shoff;
    Elf32_Shdr *cur = (Elf32_Shdr *)(base + off);
    Elf32_Shdr *str = (Elf32_Shdr *)(base + stroff);

    return (char *)elf + str->sh_offset + cur->sh_name;
}

#define SECNAME SecName

void Elf_ResolveSymbols(Elf32_Ehdr *elf) {
    s32 i;
    s32 j;
    Elf32_Shdr *symtab;

    symtab = (Elf32_Shdr *)((u8 *)elf + elf->e_shoff);
    for (i = 0; i < elf->e_shnum; i++) {
        if (strcmp(SECNAME(elf, i), D_800BBA64) == 0) {
            goto found;
        }
        symtab = (Elf32_Shdr *)((u8 *)symtab + elf->e_shentsize);
    }
    symtab = NULL;
found:
    for (i = 0; i < elf->e_shnum; i++) {
        if (strcmp(SECNAME(elf, i), D_800BBA6C) == 0) {
            break;
        }
    }
    j = 0;
    while (j < (s32)(symtab->sh_size / symtab->sh_entsize)) {
        j++;
    }
}
