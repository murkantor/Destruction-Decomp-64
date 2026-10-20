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
extern char D_800BBA74[];
extern char D_800BBA7C[];

typedef struct {
    u32 r_offset;
    u32 r_info;
} Elf32_Rel;

void Elf_Relocate(Elf32_Ehdr *elf, s32 (*resolve)(char *))
{
    Elf32_Sym *s;
    Elf32_Shdr *symtab;
    Elf32_Shdr *strtab;
    u8 *data;
    Elf32_Shdr *rel;
    Elf32_Rel *r;
    Elf32_Sym *sym;
    s32 i;
    u32 val;

    symtab = Elf_FindSection(elf, D_800BBA64);
    strtab = Elf_FindSection(elf, D_800BBA6C);
    data = (u8 *)elf + Elf_FindSection(elf, D_800BBA74)->sh_offset;
    rel = Elf_FindSection(elf, D_800BBA7C);
    r = (Elf32_Rel *)((u8 *)elf + rel->sh_offset);
    for (i = 0; i < (s32)(rel->sh_size / rel->sh_entsize); i++) {
        if ((r->r_info & 0xFF) == 2) {
            sym = (Elf32_Sym *)((u8 *)elf + symtab->sh_offset + symtab->sh_entsize * (r->r_info >> 8));
            s = sym;
            if (sym->st_shndx == 0) {
                val = resolve((char *)elf + strtab->sh_offset + sym->st_name);
                val += sym->st_value;
            } else {
                val = (u32)elf + SHDR(elf, sym->st_shndx)->sh_offset;
                val += s->st_value;
            }
            *(u32 *)(data + r->r_offset) += val;
        }
        r = (Elf32_Rel *)((u8 *)r + rel->sh_entsize);
    }
}
